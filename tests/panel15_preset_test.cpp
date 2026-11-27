// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#include "payload_keypad.hpp"

int main()
{
    using namespace engine::payload::keypad;
    const uint8_t set_panel[] = {static_cast<uint8_t>(IDENTIFIER::MAPPING),
                                 static_cast<uint8_t>(FUNCTION::SET), 7};
    content_t request{};
    request.deserialize(set_panel);
    return request.table != TABLE::PANEL15;
}
