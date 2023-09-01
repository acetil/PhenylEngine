#pragma once

#include "component/forward.h"
#include "engine/input/game_input.h"
#include "engine/phenyl_game.h"

struct Player {
    bool hasShot{false};
};

void addPlayerComponents (component::ComponentManager& manager, component::EntitySerialiser& serialiser);
void inputSetup (game::GameInput& input, const event::EventBus::SharedPtr& eventBus);
void playerUpdate (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& object);

void playerUpdatePost (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& object);