// SPDX-FileCopyrightText: 2024 Roman Koch <koch.roman@gmail.com>
// SPDX-License-Identifier: MIT
// SPDX-FileContributor: Roman Koch <koch.roman@gmail.com>
// SPDX-FileComment: Keymatrix core keymatrix handler type varikey 15sbla functionality
// SPDX-FileType: SOURCE

#include "keymatrix_handler_type_varikey_15sbla.hpp"
#include "keymatrix_core_event.hpp"
#include "keymatrix_core_handler.hpp"
#include "keypad_input.hpp"
#include "matrix15_geometry.hpp"

namespace platform::hardware
{
    static engine::keypad::key::State get_key_state(const platform::hardware::pico::event::STATE _state)
    {
        switch (_state)
        {
        case platform::hardware::pico::event::STATE::PRESS:
            return engine::keypad::key::State::PRESS;
        case platform::hardware::pico::event::STATE::RELEASE:
            return engine::keypad::key::State::RELEASE;
        default:
            break;
        }

        return engine::keypad::key::State::UNDEFINED;
    }

    void KeymatrixHandlerVarikey15SBLA::event_handler(const platform::hardware::pico::event_t _event)
    {
        const auto number = _event.switch_number;
        const auto state = get_key_state(_event.state);
        if (number && number <= engine::keypad::matrix15::capacity && state != engine::keypad::key::State::UNDEFINED)
            engine::keypad::switch_key(static_cast<engine::keypad::key::Identifier>(number - 1), state);
    }
}
