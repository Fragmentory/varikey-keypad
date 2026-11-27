// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#include <cstdlib>
#include <iostream>
#include <vector>

#include "cmd_reset_msg.hpp"
#include "serial_frame.hpp"
#include "reset_control.hpp"
#include "parameter.hpp"
#include "varikey.hpp"

namespace reset = engine::hci::cmd::reset;
static std::vector<uint8_t> reply;

uint8_t engine::serial::frame::send(uint8_t address, chunk_t const *chunk)
{
    if (address != 0x20) std::abort();
    reply.assign(chunk->space, chunk->space + chunk->size);
    return chunk->size;
}

static void check(bool condition, const char *description)
{
    if (!condition) {
        std::cerr << description << '\n';
        std::exit(1);
    }
}

static int bootsel_entries, formats, shutdowns;
void varikey_enter_bootsel() { ++bootsel_entries; }
void varikey_shutdown() { ++shutdowns; }
registry::result_t registry::parameter::param_format()
{
    ++formats;
    return registry::SUCCESS;
}

int main()
{
    uint8_t bootsel = 0x82;
    chunk_t data{&bootsel, 1};
    reset::message_t msg{};
    reset::request(&data, &msg);
    reset::confirmation(&msg);
    check(reply == std::vector<uint8_t>{0x29, 0x00, 0x82}, "RESET_CFM must echo the BOOTSEL function");

    uint8_t unknown = 0xfe;
    data = {&unknown, 1};
    reset::request(&data, &msg);
    check(msg.result == reset::RESULT::FAILURE, "unknown function must fail");

    data = {&bootsel, 0};
    reset::request(&data, &msg);
    check(msg.result == reset::RESULT::FAILURE, "truncated request must fail");
    reset::confirmation(&msg);
    check(reply == std::vector<uint8_t>{0x29, 0x01, 0xff}, "invalid request must have a failure reply");

    uint8_t overlong[] = {bootsel, 0x81};
    data = {overlong, sizeof(overlong)};
    reset::request(&data, &msg);
    check(msg.result == reset::RESULT::FAILURE, "BOOTSEL request requires exactly one HCI function byte");

    using namespace engine::reset_control;
    using engine::payload::reset::FUNCTION;
    handle(msg.content.function);
    perform();
    check(bootsel_entries == 0 && shutdowns == 0 && formats == 0, "invalid reset must do nothing");

    // USB HID output reports may have padding after the function byte.
    uint8_t padded[] = {0x82, 0, 0};
    engine::payload::reset::content_t content;
    content.deserialize(padded, sizeof(padded));
    handle(content.function);
    check(bootsel_entries == 0, "reboot must wait until the foreground loop resumes");
    handle(FUNCTION::BOOTSEL);
    perform();
    perform();
    check(bootsel_entries == 1 && formats == 0 && shutdowns == 0, "enter BOOTSEL once without formatting");

    handle(FUNCTION::SHUTDOWN);
    check(shutdowns == 1 && formats == 0, "shutdown still works");
    handle(FUNCTION::FORMAT);
    check(shutdowns == 2 && formats == 1, "format still formats and shuts down");
}
