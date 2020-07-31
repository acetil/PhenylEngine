#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/graphics_new.h"
#ifndef GAME_INIT_H
#define GAME_INIT_H
namespace game {
    GameObject* initGame (graphics::GraphicsNew* graphics);
}
#endif