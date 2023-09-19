#pragma once

#include <utility>

#include "event/event_bus.h"
#include "game/test_app.h"

namespace util {
    struct DebugConsoleEvent : public event::Event<DebugConsoleEvent> {
        inline static const std::string name = "debug_console_event";
        std::weak_ptr<event::EventBus> eventBus;
        game::TestApp* app;
        DebugConsoleEvent (std::weak_ptr<event::EventBus> _eventBus, game::TestApp* app) : eventBus{std::move(_eventBus)}, app{app} {};
    };

    void doDebugConsole (DebugConsoleEvent& event);
    void doDebugConsole (const event::EventBus::SharedPtr& eventBus, game::TestApp* app);
}
