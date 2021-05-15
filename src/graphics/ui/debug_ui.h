#include "game/game_object.h"
#include "ui_manager.h"

#ifndef DEBUG_UI_H
#define DEBUG_UI_H
namespace graphics {
    void renderDebugUi (game::GameObject* gameObject, UIManager& manager);
}
#endif