#include <cassert>

#include "engine/engine.h"
#include "engine/application.h"

using namespace phenyl;

engine::ApplicationBase::ApplicationBase (phenyl::engine::ApplicationProperties properties) : properties{properties} {}

runtime::PhenylRuntime& engine::ApplicationBase::runtime () {
    PHENYL_DASSERT(engineRuntime);
    return *engineRuntime;
}

component::ComponentManager& engine::ApplicationBase::componentManager () {
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