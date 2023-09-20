#include <cassert>

#include "engine/engine.h"
#include "engine/application.h"

using namespace engine;

component::ComponentManager& Application::componentManager () {
    assert(engine);
    return engine->getComponentManager();
}

game::GameCamera& Application::camera () {
    assert(engine);
    return engine->getCamera();
}

game::GameInput& Application::input () {
    assert(engine);
    return engine->getInput();
}

graphics::UIManager& Application::uiManager () {
    assert(engine);
    return engine->getGraphics().getUIManager();
}

void Application::setTargetFPS (double fps) {
    if (fps <= 0) {
        targetFrameTime = 0;
    } else {
        targetFrameTime = 1.0 / fps;
    }
}

component::EntitySerializer& Application::serializer () {
    return engine->getEntitySerializer();
}

void Application::setFixedTimeScale (double newTimeScale) {
    fixedTimeScale = newTimeScale;
}

void Application::pause () {
    setFixedTimeScale(0.0);
}

void Application::resume () {
    setFixedTimeScale(1.0);
}

void Application::setDebugRender (bool doRender) {
    engine->setDebugRender(doRender);
}

void Application::setProfileRender (bool doRender) {
    engine->setProfileRender(doRender);
}

void Application::dumpLevel (std::ostream& file) {
    engine->dumpLevel(file);
}
