#pragma once

#include <utility>

#include "event/event_bus.h"

namespace util {
    struct DebugConsoleEvent : public event::Event<DebugConsoleEvent> {
        inline static const std::string name = "debug_console_event";
        std::weak_ptr<event::EventBus> eventBus;
        DebugConsoleEvent (std::weak_ptr<event::EventBus> _eventBus) : eventBus{std::move(_eventBus)} {};
    };

    void doDebugConsole (DebugConsoleEvent& event);
}
