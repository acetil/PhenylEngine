#pragma once

#include "engine/game_object.h"
#include "ui_manager.h"

namespace graphics {
    void renderDebugUi (game::GameObject::SharedPtr gameObject, UIManager& manager, float deltaTime);
    void addDebugEventHandlers (const event::EventBus::SharedPtr& bus);
}
