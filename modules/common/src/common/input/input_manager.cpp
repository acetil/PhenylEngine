#include "logging/logging.h"

#include "common/detail/loggers.h"
#include "common/input/game_input.h"

using namespace phenyl::common;

static phenyl::Logger LOGGER{"INPUT_MANAGER", detail::COMMON_LOGGER};

void GameInput::addDevice (IInputDevice* device) {
    PHENYL_DASSERT(device);

    if (devices.contains(device->getDeviceId())) {
        PHENYL_LOGE(LOGGER, "Attempted to add device \"{}\" twice!", device->getDeviceId());
        return;
    }

    PHENYL_LOGD(LOGGER, "Added device \"{}\"", device->getDeviceId());
    devices.emplace(device->getDeviceId(), device);
}

InputAction GameInput::addAction (std::string_view actionId) {
    PHENYL_DASSERT(!actionId.empty());

    if (buttonBindings.contains(actionId)) {
        PHENYL_LOGE(LOGGER, "Attempted to add action \"{}\" twice!", actionId);
        return InputAction{};
    }

    PHENYL_LOGD(LOGGER, "Added action \"{}\"", actionId);
    auto it = buttonBindings.emplace(actionId, std::make_unique<ButtonInputBinding>()).first;
    return InputAction{it->second.get()};
}

InputAction GameInput::getAction (std::string_view actionId) {
    auto it = buttonBindings.find(actionId);
    if (it == buttonBindings.end()) {
        PHENYL_LOGE(LOGGER, "Failed to find input action \"{}\"", actionId);
        return InputAction{};
    }

    return InputAction{it->second.get()};
}

void GameInput::addActionBinding (std::string_view actionId, std::string_view sourceId) {
    auto bindingIt = buttonBindings.find(actionId);
    if (bindingIt == buttonBindings.end()) {
        PHENYL_LOGE(LOGGER, "Attempted to add binding to action \"{}\" that does not exist!", actionId);
        return;
    }

    auto* source = getSource(sourceId);
    if (!source) {
        PHENYL_LOGE(LOGGER, "Failed to add source \"{}\" for action \"{}\"", sourceId, actionId);
        return;
    }

    PHENYL_LOGD(LOGGER, "Added binding for \"{}\" to \"{}\"", actionId, sourceId);
    bindingIt->second->addSource(source);
}

void GameInput::update () {
    for (auto& [_, device] : devices) {
        PHENYL_TRACE(LOGGER, "Polling \"{}\"", device->getDeviceId());
        device->poll();
    }

    for (auto& [_, binding] : buttonBindings) {
        binding->poll();
    }
}

std::string_view GameInput::getName () const noexcept {
    return "InputManager";
}

const ButtonInputSource* GameInput::getSource (std::string_view sourceId) {
    auto pos = sourceId.find_first_of('.');
    if (pos == std::string_view::npos) {
        PHENYL_LOGE(LOGGER, "Invalid source id: \"{}\"", sourceId);
        return nullptr;
    }

    auto deviceId = sourceId.substr(0, pos);
    auto deviceIt = devices.find(deviceId);
    if (deviceIt == devices.end()) {
        PHENYL_LOGE(LOGGER, "Unknown device \"{}\" in source \"{}\"", deviceId, sourceId);
        return nullptr;
    }

    return deviceIt->second->getButtonSource(sourceId.substr(pos + 1));
}
