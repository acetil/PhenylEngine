#pragma once

#include "engine/game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "engine/phenyl_game.h"
#include "game_object.h"
#include "event/event.h"

namespace game {
    void initGame (graphics::PhenylGraphics graphics, game::PhenylGame gameObject, event::EventBus::SharedPtr eventBus);
}
