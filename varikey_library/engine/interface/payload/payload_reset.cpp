// SPDX-FileCopyrightText: 2024 Roman Koch <koch.roman@gmail.com>
// SPDX-License-Identifier: MIT
// SPDX-FileContributor: Roman Koch <koch.roman@gmail.com>
// SPDX-FileComment: payload reset module
// SPDX-FileType: SOURCE

#include "payload_reset.hpp"

using namespace engine::payload::reset;

static FUNCTION decode_function(uint8_t byte)
{
    switch (static_cast<FUNCTION>(byte))
    {
    case FUNCTION::SHUTDOWN:
    case FUNCTION::FORMAT:
    case FUNCTION::BOOTSEL:
        return static_cast<FUNCTION>(byte);
    default:
        return FUNCTION::UNDEFINED;
    }
}

void content_t::deserialize(uint8_t const *data, size_t size)
{
    function = data && size ? decode_function(data[0]) : FUNCTION::UNDEFINED;
}

void content_t::serialize(uint8_t **cursor) const
{
    *(*cursor)++ = static_cast<uint8_t>(decode_function(static_cast<uint8_t>(function)));
}
