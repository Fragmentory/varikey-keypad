// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace engine::keypad::layout
{
    enum class Area { soft, hard, function, left, center, right, area_count };
    struct Key
    {
        Area area;
        uint8_t usage; // USB HID Keyboard/Keypad page (0x07)
    };

    template <uint16_t Capacity, std::size_t N>
    constexpr auto describe(const std::array<Key, N> &profile)
    {
        static_assert(N <= Capacity, "Layout contains more keys than the scanner can read");
        std::array<uint16_t, static_cast<unsigned>(Area::area_count)> counts{};
        for (auto key : profile)
            ++counts[static_cast<unsigned>(key.area)];
        std::array<uint8_t, 15> bytes{1, Capacity & 0xff, Capacity >> 8};
        for (unsigned i = 0; i < counts.size(); ++i)
        {
            bytes[3 + 2 * i] = counts[i] & 0xff;
            bytes[4 + 2 * i] = counts[i] >> 8;
        }
        return bytes;
    }
}
