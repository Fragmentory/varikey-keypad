// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#include "keymatrix_handler_type_varikey_15sbla.hpp"
#include "keypad_input.hpp"
#include <vector>

namespace keypad = engine::keypad;
using Event = std::pair<keypad::key::Identifier, keypad::key::State>;
static std::vector<Event> observed;

void keypad::switch_key(key::Identifier id, key::State state)
{
    observed.emplace_back(id, state);
}

int main()
{
    using namespace platform::hardware;
    KeymatrixHandlerVarikey15SBLA handler;
    for (uint8_t sw = 1; sw <= 15; ++sw)
    {
        handler.event_handler({sw, pico::event::STATE::PRESS});
        handler.event_handler({sw, pico::event::STATE::RELEASE});
        const auto key = static_cast<keypad::key::Identifier>(sw - 1);
        if (observed != std::vector<Event>{{key, keypad::key::State::PRESS}, {key, keypad::key::State::RELEASE}})
            return 1;
        observed.clear();
    }
    handler.event_handler({0, pico::event::STATE::PRESS});
    handler.event_handler({16, pico::event::STATE::PRESS});
    handler.event_handler({1, static_cast<pico::event::STATE>(2)});
    return !observed.empty();
}
