#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>

namespace test {
    struct Player {
        phenyl::Asset<phenyl::Prefab> bulletPrefab;
        bool hasShot{false};
    };
}

void addPlayerComponents (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serialiser);
void inputSetup (phenyl::GameInput& input);
void playerUpdate (phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera);