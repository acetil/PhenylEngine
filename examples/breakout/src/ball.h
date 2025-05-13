#pragma once

#include <phenyl/asset.h>
#include <phenyl/prefab.h>
#include <phenyl/serialization.h>
#include <phenyl/world.h>

namespace breakout {
class BreakoutApp;

struct Ball {
    float maxSpeed = 1.0f;
    float appliedDrag = 0.3;
};
PHENYL_DECLARE_SERIALIZABLE(Ball)

void InitBall (BreakoutApp* app, phenyl::World& world);
} // namespace breakout
