// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#include "panel15.hpp"
#include <array>
#include <algorithm>

int main()
{
    using namespace engine::keypad::panel15;
    using engine::keypad::matrix15::switch_number;
    constexpr std::array<unsigned, 15> usages{
        0x3e,0x3d,0x3c,0x3b,0x3a,0x4b,0x4e,0x49,0x50,0x52,0x51,0x4f,0x2c,0x28,0x4a};
    for (unsigned sw = 1; sw <= usages.size(); ++sw)
    {
        const auto number = switch_number(1 + (sw - 1) % 3, 1 + (sw - 1) / 3);
        if (number != sw || keys[number - 1].usage != usages[sw - 1])
            return 1;
    }
    if (switch_number(0, 1) || switch_number(4, 1) || switch_number(1, 0) || switch_number(1, 6))
        return 2;
    constexpr std::array<uint8_t, 15> expected{1,15,0,5,0,0,0,2,0,1,0,4,0,3,0};
    std::array<uint8_t, 16> reply{};
    if (write_hid(reply.data(), 14) || write_hid(reply.data(), reply.size()) != 15 ||
        !std::equal(expected.begin(), expected.end(), reply.begin()))
        return 3;
    reply.fill(0);
    if (write_hci(1, reply) || write_hci(0, reply) != 16 || reply[0] != 0x31 ||
        !std::equal(expected.begin(), expected.end(), reply.begin() + 1))
        return 4;
    return capabilities != expected;
}
