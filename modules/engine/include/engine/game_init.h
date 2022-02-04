#pragma once

#include "engine/game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "game_object.h"

namespace game {
    detail::GameObject::SharedPtr initGame (graphics::PhenylGraphics graphics);
}
