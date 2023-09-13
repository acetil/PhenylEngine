#pragma once

#include "engine/input/game_input.h"
#include "component/forward.h"
#include "engine/engine.h"

void setupDefaultInput (game::GameInput& gameInput, const event::EventBus::SharedPtr& eventBus);

void frameInput (component::ComponentManager& manager, game::GameInput& gameInput);