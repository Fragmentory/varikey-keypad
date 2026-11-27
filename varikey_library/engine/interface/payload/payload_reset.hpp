// SPDX-FileCopyrightText: 2023 Roman Koch <koch.roman@gmail.com>
// SPDX-License-Identifier: MIT
// SPDX-FileContributor: Roman Koch <koch.roman@gmail.com>
// SPDX-FileComment: payload reset module
// SPDX-FileType: SOURCE

#pragma once

#include <cstdint>
#include <cstdlib>

#include "engine_defines.hpp"
#include "macros.hpp"
#include "payload_identifier.hpp"

namespace engine::payload::reset
{
    enum class FUNCTION : uint8_t
    {
        SHUTDOWN = common::function::CUSTOM,
        FORMAT = common::function::CUSTOM + 1,
        BOOTSEL = common::function::CUSTOM + 2,

        UNDEFINED = to_underlying(payload::IDENTIFIER::UNDEFINED),
    };

    struct content_t
    {
        FUNCTION function;

        const size_t size100() const { return 2; }
        void deserialize(uint8_t const *data, size_t size);
        void serialize(uint8_t **) const;
    };
}
