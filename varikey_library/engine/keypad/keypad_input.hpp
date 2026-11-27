// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include "keypad_key_identifier.hpp"
#include "keypad_key_state.hpp"

namespace engine::keypad
{
    void switch_key(key::Identifier identifier, key::State state);
}
