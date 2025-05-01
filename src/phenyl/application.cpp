#include <cassert>
#include <utility>

#include "phenyl/application.h"

using namespace phenyl;

engine::ApplicationBase::ApplicationBase (phenyl::ApplicationProperties properties) : m_properties{std::move(properties)} {}

core::PhenylRuntime& engine::ApplicationBase::runtime () {
    PHENYL_DASSERT(m_runtime);
    return *m_runtime;
}

core::World& engine::ApplicationBase::world () {
    return runtime().world();
}

void engine::ApplicationBase::setTargetFPS (double fps) {
    if (fps <= 0) {
        m_targetFrameTime = 0;
    } else {
        m_targetFrameTime = 1.0 / fps;
    }
}

void engine::ApplicationBase::setFixedTimeScale (double newTimeScale) {
    m_fixedTimeScale = newTimeScale;
}

void engine::ApplicationBase::pause () {
    setFixedTimeScale(0.0);
}

void engine::ApplicationBase::resume () {
    setFixedTimeScale(1.0);
}