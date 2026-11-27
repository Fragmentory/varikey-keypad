// SPDX-FileCopyrightText: 2023 Roman Koch <koch.roman@gmail.com>
// SPDX-License-Identifier: MIT
// SPDX-FileContributor: Roman Koch <koch.roman@gmail.com>
// SPDX-FileComment: commander module
// SPDX-FileType: SOURCE

#include <assert.h>
#include <stddef.h>

#include "cmd_control.hpp"
#include "cmd_feature.hpp"
#include "cmd_setting.hpp"
#include "commander.hpp"
#include "macros.hpp"
#include "panel15.hpp"
#include "serial_frame.hpp"

namespace engine::hci
{
    extern cmd::protocol::RESULT interpreter(chunk_t const *const _chunk)
    {
        assert(_chunk != NULL); // serial commander interpreter chunk null

        cmd::protocol::RESULT result = cmd::protocol::RESULT::SUCCESS;

        const COMMAND command = static_cast<COMMAND>(_chunk->space[0]);
        chunk_t sdu = {&_chunk->space[1], _chunk->size - 1};

        switch (command)
        {

#ifdef VARIKEY_PANEL15
        case COMMAND::CAPABILITIES_REQ:
        {
            keypad::panel15::HciReply bytes{};
            const auto length = keypad::panel15::write_hci(sdu.size, bytes);
            if (!length)
                return cmd::protocol::RESULT::INVALID_COMMAND;
            const chunk_t reply{bytes.data(), length};
            serial::frame::send(INTERPRETER_ADDRESS, &reply);
            break;
        }
#endif
        case COMMAND::GADGET_REQ:
            cmd::control::gadget_request(&sdu);
            break;

        case COMMAND::HASH_REQ:
            cmd::control::hash_request(&sdu);
            break;

        case COMMAND::RESET_REQ:
            cmd::control::reset_request(&sdu);
            break;

        case COMMAND::TEMPERATURE_REQ:
            cmd::control::temperature_request(&sdu);
            break;

        case COMMAND::BACKLIGHT_REQ:
            cmd::feature::backlight_request(&sdu);
            break;

        case COMMAND::DISPLAY_REQ:
            cmd::feature::display_request(&sdu);
            break;

        case COMMAND::GPIO_REQ:
            cmd::feature::gpio_request(&sdu);
            break;

        case COMMAND::KEYPAD_REQ:
            cmd::feature::keypad_request(&sdu);
            break;

        case COMMAND::IDENTITY_REQ:
            cmd::setting::identifier_request(&sdu);
            break;

        case COMMAND::PARAM_REQ:
            cmd::setting::parameter_request(&sdu);
            break;

        default:

            result = cmd::protocol::RESULT::INVALID_COMMAND;
            break;
        }

        return result;
    }
}
