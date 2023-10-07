#pragma once

#include <phenyl/component.h>
#include <phenyl/serialization.h>

#include "breakout.h"

namespace breakout {
    struct Tile {
        int points = 10;
        int health = 1;
        phenyl::Asset<phenyl::Prefab> emitter;
    };

    PHENYL_SERIALIZE(Tile, {
        PHENYL_MEMBER(points);
        PHENYL_MEMBER(health);
        PHENYL_MEMBER(emitter);
    })

    void initTile (breakout::BreakoutApp* app, phenyl::ComponentManager& componentManager);
}