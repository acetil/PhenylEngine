#pragma once

#include <phenyl/asset.h>
#include <phenyl/prefab.h>
#include <phenyl/serialization.h>
#include <phenyl/world.h>

namespace test {
class TestApp;

struct Bullet {
    phenyl::Asset<phenyl::Prefab> particlePrefab;
};
PHENYL_DECLARE_SERIALIZABLE(Bullet)

void InitBullet (TestApp* app, phenyl::World& world);
} // namespace test
