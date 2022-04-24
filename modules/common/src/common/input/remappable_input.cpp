#include "common/input/remappable_input.h"

#include "logging/logging.h"

using namespace common;

void RemappableInput::addInputSource (const std::shared_ptr<InputSource>& source) {
    inputSources.emplace_back(source);
}

InputAction RemappableInput::addInputMapping (const std::string& actionName, const std::string& inputName) {
    InputAction inputAction;

    if (!actionMap.contains(actionName)) {
        uint32_t index = actions.size() + 1;
        actions.push_back({});
        actionMap[actionName] = InputAction{index};
    }

    inputAction = actionMap.at(actionName);

    detail::SourceAction sourceAction;
    if (!(sourceAction = getSourceAction(inputName))) {
        logging::log(LEVEL_ERROR, "Unable to find input {}!", inputName);
        return {};
    }

    actions[inputAction.actionIndex - 1] = sourceAction;

    return inputAction;
}

InputAction RemappableInput::getInputAction (const std::string& actionName) {
    if (!actionMap.contains(actionName)) {
        logging::log(LEVEL_ERROR, "Unable to find input action {}!", actionName);
        return {};
    }

    return actionMap.at(actionName);
}

void RemappableInput::setInputMapping (const InputAction& action, const std::string& inputName) {
    if (!action || (action.actionIndex > actions.size())) {
        logging::log(LEVEL_ERROR, "Invalid input action!");
        return;
    }

    auto sourceAction = getSourceAction(inputName);

    if (!sourceAction) {
        logging::log(LEVEL_ERROR, "Unable to find input {}!", inputName);
        return;
    }

    actions[action.actionIndex - 1] = sourceAction;
}

detail::SourceAction RemappableInput::getSourceAction (const std::string& inputName) {
    size_t sourceIndex = 0;
    long inputId = -1;
    for (auto& source : inputSources) {
        if ((inputId = source->getInputNum(inputName)) != -1) {
            break;
        }
        sourceIndex++;
    }

    if (inputId < 0) {
        return {};
    }
    return {sourceIndex, inputId};
}

bool RemappableInput::isActive (const InputAction& action) {
    if (!action || (action.actionIndex > actions.size())) {
        logging::log(LEVEL_ERROR, "Invalid input action!");
        return false;
    }

    detail::SourceAction sourceAction = actions[action.actionIndex - 1];

    return inputSources.at(sourceAction.sourceIndex)->isDown(sourceAction.actionIndex);
}
