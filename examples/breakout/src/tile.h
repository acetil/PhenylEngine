#pragma once

#include <phenyl/audio.h>
#include <phenyl/component.h>
#include <phenyl/serialization.h>

#include "breakout.h"

namespace breakout {
    struct Tile {
        int points = 10;
        int health = 1;
        phenyl::Asset<phenyl::Prefab> emitter;
        phenyl::Asset<phenyl::AudioSample> breakSample;
    };
    PHENYL_DECLARE_SERIALIZABLE(Tile)

    struct Floor {
        phenyl::Asset<phenyl::AudioSample> sample;
    };
    PHENYL_DECLARE_SERIALIZABLE(Floor)

    void InitTile (breakout::BreakoutApp* app, phenyl::World& world);
}