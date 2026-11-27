// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#pragma once

#include "payload_reset.hpp"

namespace engine::reset_control
{
    void handle(payload::reset::FUNCTION function);
    void perform();
}
