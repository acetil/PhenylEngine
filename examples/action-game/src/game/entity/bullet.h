#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/prefab.h>

namespace test {
    class TestApp;
    struct Bullet {
        phenyl::Asset<phenyl::Prefab> particlePrefab;
    };

    void addBulletSignals (TestApp* app, phenyl::ComponentManager& manager);
}