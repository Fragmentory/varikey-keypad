// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>

namespace engine::keypad::matrix15
{
    constexpr unsigned drive_lines = 3;
    constexpr unsigned sense_lines = 5;
    constexpr unsigned capacity = drive_lines * sense_lines;

    constexpr uint8_t switch_number(unsigned drive, unsigned sense)
    {
        if (!drive || drive > drive_lines || !sense || sense > sense_lines)
            return 0;
        return (sense - 1) * drive_lines + drive;
    }
}
