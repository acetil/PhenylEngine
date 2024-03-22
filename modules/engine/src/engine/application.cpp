#include <cassert>

#include "engine/engine.h"
#include "engine/application.h"

using namespace phenyl;

engine::Application::Application (phenyl::engine::ApplicationProperties properties) : properties{properties} {}

runtime::PhenylRuntime& engine::Application::runtime () {
    PHENYL_DASSERT(engine);
    return engine->getRuntime();
}

component::ComponentManager& engine::Application::componentManager () {
    PHENYL_DASSERT(engine);
    return runtime().manager();
}

/*graphics::Camera& engine::Application::camera () {
    PHENYL_DASSERT(engine);
    return engine->getRuntime().resource<graphics::Camera>(); // TODO
}

game::GameInput& engine::Application::input () {
    PHENYL_DASSERT(engine);
    return engine->getRuntime().resource<game::GameInput>(); // TODO
}

graphics::UIManager& engine::Application::uiManager () {
    PHENYL_DASSERT(engine);
    return engine->getRuntime().resource<graphics::UIManager>(); // TODO
}*/

void engine::Application::setTargetFPS (double fps) {
    if (fps <= 0) {
        targetFrameTime = 0;
    } else {
        targetFrameTime = 1.0 / fps;
    }
}

void engine::Application::setFixedTimeScale (double newTimeScale) {
    fixedTimeScale = newTimeScale;
}

void engine::Application::pause () {
    setFixedTimeScale(0.0);
}

void engine::Application::resume () {
    setFixedTimeScale(1.0);
}