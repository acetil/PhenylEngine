#pragma once

#include "phenyl/world.h"

#include <phenyl/asset.h>
#include <phenyl/audio.h>
#include <phenyl/prefab.h>

namespace breakout {
class BreakoutApp;

struct Wall {
    std::shared_ptr<phenyl::Prefab> emitter;
    std::shared_ptr<phenyl::AudioSample> bounceSample;
};
PHENYL_DECLARE_SERIALIZABLE(Wall)

void InitWall (BreakoutApp* app, phenyl::World& world);
} // namespace breakout
