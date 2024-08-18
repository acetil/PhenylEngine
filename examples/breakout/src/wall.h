#pragma once

#include <phenyl/audio.h>
#include <phenyl/asset.h>
#include <phenyl/prefab.h>
#include "phenyl/component.h"

namespace breakout {
    class BreakoutApp;

    struct Wall {
        phenyl::Asset<phenyl::Prefab> emitter;
        phenyl::Asset<phenyl::AudioSample> bounceSample;
    };

    PHENYL_SERIALIZE(Wall, {
        PHENYL_MEMBER(emitter);
        PHENYL_MEMBER_NAMED(bounceSample, "bounce_sample");
    })

    void InitWall (BreakoutApp* app, phenyl::World& world);
}