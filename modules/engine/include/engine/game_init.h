#pragma once

#include "engine/game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"

namespace game {
    GameObject::SharedPtr initGame (graphics::PhenylGraphics graphics);
}
