#pragma once

#include "component/forward.h"
#include "engine/input/game_input.h"
#include "common/assets/asset.h"
#include "engine/game_camera.h"

namespace test {
    struct Player {
        phenyl::common::Asset<phenyl::component::Prefab> bulletPrefab;
        bool hasShot{false};
    };
}

void addPlayerComponents (phenyl::component::ComponentManager& manager, phenyl::component::EntitySerializer& serialiser);
void inputSetup (phenyl::game::GameInput& input);
void playerUpdate (phenyl::component::ComponentManager& manager, phenyl::game::GameInput& input, phenyl::game::GameCamera& camera);