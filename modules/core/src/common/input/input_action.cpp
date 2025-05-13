#include "core/input/input_action.h"

#include "core/input/input_source.h"
#include "logging/logging.h"

#include <numeric>

using namespace phenyl::core;

void ButtonInputBinding::addSource (const ButtonInputSource* source) {
    PHENYL_DASSERT(source);
    m_sources.emplace_back(source);
}

void ButtonInputBinding::poll () {
    bool newState = false;
    for (auto* source : m_sources) {
        newState = newState || source->state();
    }

    m_state = newState;
}

InputAction::InputAction () = default;

InputAction::InputAction (const ButtonInputBinding* binding) : m_binding{binding} {}
