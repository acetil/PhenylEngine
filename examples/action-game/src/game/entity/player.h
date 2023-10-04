#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>

namespace test {
    class TestApp;
    struct Player {
        phenyl::Asset<phenyl::Prefab> bulletPrefab;
        bool hasShot{false};
    };
}

void addPlayerComponents (test::TestApp* app);
void inputSetup (phenyl::GameInput& input);
void playerUpdate (phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera);