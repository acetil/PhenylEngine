#pragma once

#include "breakout.h"

#include <phenyl/audio.h>
#include <phenyl/serialization.h>
#include <phenyl/world.h>

namespace breakout {
struct Tile {
    int points = 10;
    int health = 1;
    std::shared_ptr<phenyl::Prefab> emitter;
    std::shared_ptr<phenyl::AudioSample> breakSample;
};
PHENYL_DECLARE_SERIALIZABLE(Tile)

struct Floor {
    std::shared_ptr<phenyl::AudioSample> sample;
};
PHENYL_DECLARE_SERIALIZABLE(Floor)

void InitTile (breakout::BreakoutApp* app, phenyl::World& world);
} // namespace breakout
