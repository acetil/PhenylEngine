module;

#include "engine/input/game_input.h"
#include "common/input/remappable_input.h"

export module phenyl.input;

export namespace phenyl {
    using GameInput = phenyl::game::GameInput;
    using InputAction = phenyl::common::InputAction;
}