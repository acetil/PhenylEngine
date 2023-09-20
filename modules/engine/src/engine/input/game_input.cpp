#include "engine/input/game_input.h"
#include "graphics/renderers/renderer.h"

#include <utility>

using namespace phenyl::game;

phenyl::common::InputAction GameInput::mapInput (const std::string& actionName, const std::string& inputName) {
    return inputSource->addInputMapping(actionName, inputName);
}

phenyl::common::InputAction GameInput::getInput (const std::string& actionName) {
    return inputSource->getInputAction(actionName);
}

bool GameInput::isDown (const common::InputAction& action) {
    return inputSource->isActive(action);
}

void GameInput::poll () {
}

GameInput::GameInput ()  {
    inputSource = std::make_unique<common::RemappableInput>();
}

void GameInput::addInputSources (const std::vector<std::shared_ptr<common::InputSource>>& sources) {
    for (const auto& i : sources) {
        inputSource->addInputSource(i);
    }
}

glm::vec2 GameInput::cursorPos () const {
    return renderer->getMousePos();
}

glm::vec2 GameInput::screenSize () const {
    return renderer->getScreenSize();
}

void GameInput::setRenderer (graphics::Renderer* renderer) {
    this->renderer = renderer;
}
