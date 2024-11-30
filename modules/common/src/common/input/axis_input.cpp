#include "logging/logging.h"

#include "core/input/axis_action.h"

using namespace phenyl::core;

Axis2DBinding::Axis2DBinding (bool normalised) : normalised{normalised} {}

void Axis2DBinding::addButtonSource (const ButtonInputSource* source, glm::vec2 sourceVec) {
    PHENYL_DASSERT(source);
    buttonSources.emplace_back(source, sourceVec);
}

void Axis2DBinding::addAxisSource (const Axis2DInputSource* source) {
    PHENYL_DASSERT(source);
    axisSources.emplace_back(source);
}

void Axis2DBinding::poll () {
    currState = {0, 0};

    for (const auto& [source, vec] : buttonSources) {
        if (source->state()) {
            currState += vec;
        }
    }

    for (const auto* source : axisSources) {
        currState += source->state();
    }

    if (normalised) {
        currState = util::SafeNormalize(currState);
    }
}
