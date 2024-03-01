#include <cassert>

#include "engine/engine.h"
#include "engine/application.h"

using namespace phenyl;

engine::Application::Application (phenyl::engine::ApplicationProperties properties) : properties{properties} {}

component::ComponentManager& engine::Application::componentManager () {
    PHENYL_DASSERT(engine);
    return engine->getComponentManager();
}

game::GameCamera& engine::Application::camera () {
    PHENYL_DASSERT(engine);
    return engine->getCamera();
}

game::GameInput& engine::Application::input () {
    PHENYL_DASSERT(engine);
    return engine->getInput();
}

graphics::UIManager& engine::Application::uiManager () {
    PHENYL_DASSERT(engine);
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

audio::AudioSystem& engine::Application::audio () {
    return engine->getAudio();
}
