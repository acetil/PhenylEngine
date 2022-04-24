#include "engine/input/game_input.h"

#include <utility>

using namespace game;

common::InputAction game::GameInput::mapInput (const std::string& actionName, const std::string& inputName) {
    return inputSource->addInputMapping(actionName, inputName);
}

common::InputAction GameInput::getInput (const std::string& actionName) {
    return inputSource->getInputAction(actionName);
}

bool GameInput::isDown (const common::InputAction& action) {
    return inputSource->isActive(action);
}

void GameInput::poll () {
    for (auto& eAct : eventActions) {
        eAct->poll(inputSource, eventBus);
    }
}

GameInput::GameInput () {
    inputSource = std::make_unique<common::RemappableInput>();
}

void GameInput::addInputSources (const std::vector<std::shared_ptr<common::InputSource>>& sources) {
    for (const auto& i : sources) {
        inputSource->addInputSource(i);
    }
}

void GameInput::setEventBus (event::EventBus::SharedPtr _eventBus) {
    eventBus = std::move(_eventBus);
}
