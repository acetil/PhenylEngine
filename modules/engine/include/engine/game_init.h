#include "engine/game_object.h"
#include "graphics/graphics.h"
#ifndef GAME_INIT_H
#define GAME_INIT_H
namespace game {
    GameObject::SharedPtr initGame (const graphics::Graphics::SharedPtr& graphics);
}
#endif