#pragma once

//#include "engine/game_object.h"
#include "engine/phenyl_game.h"
#include "ui_manager.h"

namespace graphics {
    void renderDebugUi (game::PhenylGame gameObject, UIManager& manager, float deltaTime);
    void addDebugEventHandlers (const event::EventBus::SharedPtr& bus);
}
