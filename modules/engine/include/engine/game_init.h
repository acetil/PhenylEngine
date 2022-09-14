#pragma once

#include "engine/game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "engine/phenyl_game.h"
#include "game_object.h"
#include "event/event.h"

namespace game {
    void initGame (const graphics::PhenylGraphics& graphics, game::PhenylGame gameObject, const event::EventBus::SharedPtr& eventBus);
}
