#include <numeric>

#include "core/input/input_action.h"
#include "core/input/input_source.h"
#include "logging/logging.h"

using namespace phenyl::core;

void ButtonInputBinding::addSource (const ButtonInputSource* source) {
    PHENYL_DASSERT(source);
    sources.emplace_back(source);
}

void ButtonInputBinding::poll () {
    bool newState = false;
    for (auto* source : sources) {
        newState = newState || source->state();
    }

    currState = newState;
}

InputAction::InputAction () = default;

InputAction::InputAction (const ButtonInputBinding* binding) : binding{binding} {}
