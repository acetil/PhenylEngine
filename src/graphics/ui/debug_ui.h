#include "game/game_object.h"
#include "ui_manager.h"

#ifndef DEBUG_UI_H
#define DEBUG_UI_H
namespace graphics {
    void renderDebugUi (game::GameObject::SharedPtr gameObject, UIManager& manager, float deltaTime);
    void addDebugEventHandlers (const event::EventBus::SharedPtr& bus);
}
#endif