#pragma once

#include "event/event_bus.h"
#include "ui_manager.h"

namespace graphics {
    void renderDebugUi (UIManager& manager, float deltaTime);
    void addDebugEventHandlers (const event::EventBus::SharedPtr& bus);
}
