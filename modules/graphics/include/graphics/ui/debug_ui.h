#pragma once

#include "engine/game_object.h"
#include "ui_manager.h"
#include "game_object.h"

namespace graphics {
    void renderDebugUi (game::detail::GameObject::SharedPtr gameObject, UIManager& manager, float deltaTime);
    void addDebugEventHandlers (const event::EventBus::SharedPtr& bus);
}
