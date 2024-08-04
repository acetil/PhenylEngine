#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/prefab.h>

namespace test {
    class TestApp;
    struct Bullet {
        phenyl::Asset<phenyl::Prefab> particlePrefab;
    };

    void InitBullet (TestApp* app, phenyl::ComponentManager& manager);
}