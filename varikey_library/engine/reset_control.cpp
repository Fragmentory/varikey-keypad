// SPDX-FileCopyrightText: 2026 Roman Koch
// SPDX-License-Identifier: MIT
#include "reset_control.hpp"

#include <atomic>
#include "parameter.hpp"
#include "varikey.hpp"

namespace engine::reset_control
{
    // The timer handler is the producer; only the core-0 foreground loop consumes.
    static std::atomic<bool> bootsel_pending{false};

    void handle(payload::reset::FUNCTION function)
    {
        using payload::reset::FUNCTION;
        switch (function)
        {
        case FUNCTION::BOOTSEL:
            bootsel_pending.store(true, std::memory_order_relaxed);
            break;
        case FUNCTION::FORMAT:
            registry::parameter::param_format();
            [[fallthrough]];
        case FUNCTION::SHUTDOWN:
            varikey_shutdown();
            break;
        default:
            break;
        }
    }

    void perform()
    {
        if (bootsel_pending.load(std::memory_order_relaxed))
        {
            bootsel_pending.store(false, std::memory_order_relaxed);
            varikey_enter_bootsel();
        }
    }
}
