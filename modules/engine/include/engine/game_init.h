#pragma once

#include "engine/game_object.h"
#include "graphics/graphics.h"

namespace game {
    GameObject::SharedPtr initGame (const graphics::Graphics::SharedPtr& graphics);
}
