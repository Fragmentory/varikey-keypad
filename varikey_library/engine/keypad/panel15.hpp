// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "matrix15_geometry.hpp"

namespace engine::keypad::panel15
{
    enum Area { soft, hard, function, left, center, right, area_count };
    struct Key
    {
        Area area;
        uint8_t usage; // USB HID Keyboard/Keypad page (0x07)
    };

    // Indexed by electrical switch number minus one.
    inline constexpr std::array keys{
        Key{soft, 0x3e}, Key{soft, 0x3d}, Key{soft, 0x3c}, Key{soft, 0x3b}, Key{soft, 0x3a},
        Key{function, 0x4b}, Key{function, 0x4e}, Key{left, 0x49},
        Key{center, 0x50}, Key{center, 0x52}, Key{center, 0x51}, Key{center, 0x4f},
        Key{right, 0x2c}, Key{right, 0x28}, Key{right, 0x4a}
    };
    static_assert(keys.size() <= matrix15::capacity);

    inline constexpr auto capabilities = [] {
        std::array<uint16_t, area_count> counts{};
        for (auto key : keys)
            ++counts[key.area];
        std::array<uint8_t, 15> bytes{1, matrix15::capacity & 0xff, matrix15::capacity >> 8};
        for (unsigned i = 0; i < counts.size(); ++i)
        {
            bytes[3 + 2*i] = counts[i] & 0xff;
            bytes[4 + 2*i] = counts[i] >> 8;
        }
        return bytes;
    }();

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
