#pragma once

#include "engine/input/game_input.h"
#include "component/forward.h"
#include "engine/engine.h"
#include "test_app.h"

void setupDefaultInput (game::GameInput& gameInput, const event::EventBus::SharedPtr& eventBus, game::TestApp* app);

void frameInput (component::ComponentManager& manager, game::GameInput& gameInput);