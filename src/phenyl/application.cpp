#include <cassert>
#include <utility>

#include "phenyl/application.h"

using namespace phenyl;

engine::ApplicationBase::ApplicationBase (phenyl::ApplicationProperties properties) : properties{std::move(properties)} {}

runtime::PhenylRuntime& engine::ApplicationBase::runtime () {
    PHENYL_DASSERT(engineRuntime);
    return *engineRuntime;
}

component::EntityComponentManager& engine::ApplicationBase::componentManager () {
    return runtime().manager();
}

void engine::ApplicationBase::setTargetFPS (double fps) {
    if (fps <= 0) {
        targetFrameTime = 0;
    } else {
        targetFrameTime = 1.0 / fps;
    }
}

void engine::ApplicationBase::setFixedTimeScale (double newTimeScale) {
    fixedTimeScale = newTimeScale;
}

void engine::ApplicationBase::pause () {
    setFixedTimeScale(0.0);
}

void engine::ApplicationBase::resume () {
    setFixedTimeScale(1.0);
}