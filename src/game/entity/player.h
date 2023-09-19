#pragma once

#include "component/forward.h"
#include "engine/input/game_input.h"
#include "common/assets/asset.h"
#include "engine/game_camera.h"

namespace game {
    struct Player {
        common::Asset<component::Prefab> bulletPrefab;
        bool hasShot{false};
    };
}

void addPlayerComponents (component::ComponentManager& manager, component::EntitySerializer& serialiser);
void inputSetup (game::GameInput& input);
void playerUpdate (component::ComponentManager& manager, game::GameInput& input, game::GameCamera& camera);