// SPDX-FileCopyrightText: 2023 Roman Koch <koch.roman@gmail.com>
// SPDX-License-Identifier: MIT
// SPDX-FileContributor: Roman Koch <koch.roman@gmail.com>
// SPDX-FileComment: Keymatrix core keymatrix core event functionality
// SPDX-FileType: SOURCE

#pragma once

#include <stdint.h>

namespace platform::hardware::pico
{
    namespace event
    {
        enum class STATE : uint8_t
        {
            RELEASE = 0,
            PRESS = 1,
        };
    }

    struct event_t
    {
        uint8_t switch_number;
        event::STATE state;
    };
}
