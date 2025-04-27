#include "logging/logging.h"

#include "core/input/axis_action.h"

using namespace phenyl::core;

Axis2DBinding::Axis2DBinding (bool normalised) : m_normalised{normalised} {}

void Axis2DBinding::addButtonSource (const ButtonInputSource* source, glm::vec2 sourceVec) {
    PHENYL_DASSERT(source);
    m_buttonSources.emplace_back(source, sourceVec);
}

void Axis2DBinding::addAxisSource (const Axis2DInputSource* source) {
    PHENYL_DASSERT(source);
    m_axisSources.emplace_back(source);
}

void Axis2DBinding::poll () {
    m_state = {0, 0};

    for (const auto& [source, vec] : m_buttonSources) {
        if (source->state()) {
            m_state += vec;
        }
    }

    for (const auto* source : m_axisSources) {
        m_state += source->state();
    }

    if (m_normalised) {
        m_state = util::SafeNormalize(m_state);
    }
}
