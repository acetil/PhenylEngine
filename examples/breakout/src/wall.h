#pragma once

#include "phenyl/world.h"

#include <phenyl/asset.h>
#include <phenyl/audio.h>
#include <phenyl/prefab.h>

namespace breakout {
class BreakoutApp;

struct Wall {
    phenyl::Asset<phenyl::Prefab> emitter;
    phenyl::Asset<phenyl::AudioSample> bounceSample;
};
PHENYL_DECLARE_SERIALIZABLE(Wall)

void InitWall (BreakoutApp* app, phenyl::World& world);
} // namespace breakout
