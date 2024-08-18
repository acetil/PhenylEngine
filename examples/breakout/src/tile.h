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

    PHENYL_SERIALIZE(Tile, {
        PHENYL_MEMBER(points);
        PHENYL_MEMBER(health);
        PHENYL_MEMBER(emitter);
        PHENYL_MEMBER_NAMED(breakSample, "break_sample");
    })

    struct Floor {
        phenyl::Asset<phenyl::AudioSample> sample;
    };

    PHENYL_SERIALIZE(Floor, {
        PHENYL_MEMBER(sample);
    })

    void InitTile (breakout::BreakoutApp* app, phenyl::World& world);
}