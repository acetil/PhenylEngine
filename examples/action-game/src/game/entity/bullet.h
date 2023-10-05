#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/prefab.h>

namespace test {
    struct Bullet {
        phenyl::Asset<phenyl::Prefab> particlePrefab;
    };

    void addBulletSignals (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serialiser);
}