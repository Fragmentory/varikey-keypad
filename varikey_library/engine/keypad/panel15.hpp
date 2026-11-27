// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "keypad_layout.hpp"
#include "matrix15_geometry.hpp"

namespace engine::keypad::panel15
{
    using layout::Area;
    using layout::Key;

    // Indexed by electrical switch number minus one.
    inline constexpr std::array keys{
        Key{Area::soft, 0x3e}, Key{Area::soft, 0x3d}, Key{Area::soft, 0x3c}, Key{Area::soft, 0x3b}, Key{Area::soft, 0x3a},
        Key{Area::function, 0x4b}, Key{Area::function, 0x4e}, Key{Area::left, 0x49},
        Key{Area::center, 0x50}, Key{Area::center, 0x52}, Key{Area::center, 0x51}, Key{Area::center, 0x4f},
        Key{Area::right, 0x2c}, Key{Area::right, 0x28}, Key{Area::right, 0x4a}
    };
    inline constexpr auto capabilities = layout::describe<matrix15::capacity>(keys);

    constexpr uint8_t report_id = 0xe1;
    inline uint16_t write_hid(uint8_t *buffer, std::size_t size)
    {
        if (size < capabilities.size())
            return 0;
        std::copy(capabilities.begin(), capabilities.end(), buffer);
        return capabilities.size();
    }

    using HciReply = std::array<uint8_t, 1 + capabilities.size()>;
    inline uint16_t write_hci(std::size_t request_payload_size, HciReply &reply)
    {
        if (request_payload_size != 0)
            return 0;
        reply[0] = 0x31;
        std::copy(capabilities.begin(), capabilities.end(), reply.begin() + 1);
        return reply.size();
    }

}
