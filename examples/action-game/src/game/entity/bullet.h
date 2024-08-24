#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/prefab.h>
#include <phenyl/serialization.h>

namespace test {
    class TestApp;
    struct Bullet {
        phenyl::Asset<phenyl::Prefab> particlePrefab;
    };
    PHENYL_DECLARE_SERIALIZABLE(Bullet)

    void InitBullet (TestApp* app, phenyl::World& world);
}