#include "common/input/remappable_input.h"

#include "common/detail/loggers.h"
#include "logging/logging.h"

using namespace phenyl::common;

static phenyl::Logger LOGGER{"REMAPPABLE_INPUT", detail::COMMON_LOGGER};

void RemappableInput::addInputSource (const std::shared_ptr<InputSource>& source) {
    inputSources.emplace_back(source);
}

InputAction RemappableInput::addInputMapping (const std::string& actionName, const std::string& inputName) {
    if (!actionMap.contains(actionName)) {
        uint32_t index = actions.size() + 1;
        actions.push_back({});
        actionMap[actionName] = InputAction{index};
    }

    const InputAction inputAction = actionMap.at(actionName);

    detail::SourceAction sourceAction;
    if (!((sourceAction = getSourceAction(inputName)))) {
        PHENYL_LOGE(LOGGER, "Unable to find input {}!", inputName);
        return {};
    }

    actions[inputAction.actionIndex - 1] = sourceAction;

    return inputAction;
}

InputAction RemappableInput::getInputAction (const std::string& actionName) {
    if (!actionMap.contains(actionName)) {
        PHENYL_LOGE(LOGGER, "Unable to find input action {}!", actionName);
        return {};
    }

    return actionMap.at(actionName);
}

void RemappableInput::setInputMapping (const InputAction& action, const std::string& inputName) {
    if (!action || (action.actionIndex > actions.size())) {
        PHENYL_LOGE(LOGGER, "Invalid input action!");
        return;
    }

    auto sourceAction = getSourceAction(inputName);

    if (!sourceAction) {
        PHENYL_LOGE(LOGGER, "Unable to find input {}!", inputName);
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
        PHENYL_LOGE(LOGGER, "Invalid input action!");
        return false;
    }

    detail::SourceAction sourceAction = actions[action.actionIndex - 1];

    return inputSources.at(sourceAction.sourceIndex)->isDown(sourceAction.actionIndex);
}

uint32_t RemappableInput::getActionIndex (InputAction action) {
    return action.actionIndex;
}
