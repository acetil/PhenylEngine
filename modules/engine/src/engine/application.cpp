#include <cassert>

#include "engine/engine.h"
#include "engine/application.h"

using namespace phenyl;

component::ComponentManager& engine::Application::componentManager () {
    assert(engine);
    return engine->getComponentManager();
}

game::GameCamera& engine::Application::camera () {
    assert(engine);
    return engine->getCamera();
}

game::GameInput& engine::Application::input () {
    assert(engine);
    return engine->getInput();
}

graphics::UIManager& engine::Application::uiManager () {
    assert(engine);
    return engine->getGraphics().getUIManager();
}

void engine::Application::setTargetFPS (double fps) {
    if (fps <= 0) {
        targetFrameTime = 0;
    } else {
        targetFrameTime = 1.0 / fps;
    }
}

component::EntitySerializer& engine::Application::serializer () {
    return engine->getEntitySerializer();
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

void engine::Application::setDebugRender (bool doRender) {
    engine->setDebugRender(doRender);
}

void engine::Application::setProfileRender (bool doRender) {
    engine->setProfileRender(doRender);
}

void engine::Application::dumpLevel (std::ostream& file) {
    engine->dumpLevel(file);
}
