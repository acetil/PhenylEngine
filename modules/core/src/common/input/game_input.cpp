#include "logging/logging.h"

#include "core/detail/loggers.h"
#include "core/input/game_input.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"GAME_INPUT", detail::COMMON_LOGGER};

void GameInput::addDevice (IInputDevice* device) {
    PHENYL_DASSERT(device);

    if (m_devices.contains(device->getDeviceId())) {
        PHENYL_LOGE(LOGGER, "Attempted to add device \"{}\" twice!", device->getDeviceId());
        return;
    }

    PHENYL_LOGD(LOGGER, "Added device \"{}\"", device->getDeviceId());
    m_devices.emplace(device->getDeviceId(), device);
}

InputAction GameInput::addAction (std::string_view actionId) {
    PHENYL_DASSERT(!actionId.empty());

    if (m_buttonBindings.contains(actionId)) {
        PHENYL_LOGE(LOGGER, "Attempted to add action \"{}\" twice!", actionId);
        return InputAction{};
    }

    PHENYL_LOGD(LOGGER, "Added action \"{}\"", actionId);
    auto it = m_buttonBindings.emplace(actionId, std::make_unique<ButtonInputBinding>()).first;
    return InputAction{it->second.get()};
}

InputAction GameInput::getAction (std::string_view actionId) {
    auto it = m_buttonBindings.find(actionId);
    if (it == m_buttonBindings.end()) {
        PHENYL_LOGE(LOGGER, "Failed to find input action \"{}\"", actionId);
        return InputAction{};
    }

    return InputAction{it->second.get()};
}

void GameInput::addActionBinding (std::string_view actionId, std::string_view sourceId) {
    auto bindingIt = m_buttonBindings.find(actionId);
    if (bindingIt == m_buttonBindings.end()) {
        PHENYL_LOGE(LOGGER, "Attempted to add binding to action \"{}\" that does not exist!", actionId);
        return;
    }

    auto* source = getButtonSource(sourceId);
    if (!source) {
        PHENYL_LOGE(LOGGER, "Failed to add source \"{}\" for action \"{}\"", sourceId, actionId);
        return;
    }

    PHENYL_LOGD(LOGGER, "Added binding for \"{}\" to \"{}\"", actionId, sourceId);
    bindingIt->second->addSource(source);
}

Axis2DInput GameInput::addAxis2D (std::string_view inputId, bool normalised) {
    PHENYL_DASSERT(!inputId.empty());

    if (m_axis2DBindings.contains(inputId)) {
        PHENYL_LOGE(LOGGER, "Attempted to add axis input \"{}\" twice!", inputId);
        return Axis2DInput{};
    }

    PHENYL_LOGD(LOGGER, "Added Axis2D input \"{}\"", inputId);
    auto it = m_axis2DBindings.emplace(inputId, std::make_unique<Axis2DBinding>(normalised)).first;
    return Axis2DInput{it->second.get()};
}

Axis2DInput GameInput::getAxis2D (std::string_view inputId) {
    auto it = m_axis2DBindings.find(inputId);
    if (it == m_axis2DBindings.end()) {
        PHENYL_LOGE(LOGGER, "Failed to find Axis2D input \"{}\"", inputId);
        return Axis2DInput{};
    }

    return Axis2DInput{it->second.get()};
}

void GameInput::addButtonAxis2DBinding (std::string_view inputId, std::string_view sourceId, glm::vec2 buttonAxis) {
    auto bindingIt = m_axis2DBindings.find(inputId);
    if (bindingIt == m_axis2DBindings.end()) {
        PHENYL_LOGE(LOGGER, "Attempted to add binding to Axis2D input \"{}\" that does not exist!", inputId);
        return;
    }

    auto* source = getButtonSource(sourceId);
    if (!source) {
        PHENYL_LOGE(LOGGER, "Failed to add button source \"{}\" for Axis2D input \"{}\"", sourceId, inputId);
        return;
    }

    PHENYL_LOGD(LOGGER, "Added button binding for \"{}\" to \"{}\" with vec={}", inputId, sourceId, buttonAxis);
    bindingIt->second->addButtonSource(source, buttonAxis);
}

void GameInput::addAxis2DBinding (std::string_view inputId, std::string_view sourceId) {
    auto bindingIt = m_axis2DBindings.find(inputId);
    if (bindingIt == m_axis2DBindings.end()) {
        PHENYL_LOGE(LOGGER, "Attempted to add binding to Axis2D input \"{}\" that does not exist!", inputId);
        return;
    }

    auto* source = getAxis2DSource(sourceId);
    if (!source) {
        PHENYL_LOGE(LOGGER, "Failed to add Axis2D source \"{}\" for Axis2D input \"{}\"", sourceId, inputId);
        return;
    }

    PHENYL_LOGD(LOGGER, "Added Axis2D binding for \"{}\" to \"{}\"", inputId, sourceId);
    bindingIt->second->addAxisSource(source);
}

void GameInput::update () {
    for (auto& [_, device] : m_devices) {
        PHENYL_TRACE(LOGGER, "Polling \"{}\"", device->getDeviceId());
        device->poll();
    }

    for (auto& [_, binding] : m_buttonBindings) {
        binding->poll();
    }

    for (auto& [_, binding] : m_axis2DBindings) {
        binding->poll();
    }
}

std::string_view GameInput::getName () const noexcept {
    return "InputManager";
}

const ButtonInputSource* GameInput::getButtonSource (std::string_view sourceId) {
    auto pos = sourceId.find_first_of('.');
    if (pos == std::string_view::npos) {
        PHENYL_LOGE(LOGGER, "Invalid source id: \"{}\"", sourceId);
        return nullptr;
    }

    auto deviceId = sourceId.substr(0, pos);
    auto deviceIt = m_devices.find(deviceId);
    if (deviceIt == m_devices.end()) {
        PHENYL_LOGE(LOGGER, "Unknown device \"{}\" in source \"{}\"", deviceId, sourceId);
        return nullptr;
    }

    return deviceIt->second->getButtonSource(sourceId.substr(pos + 1));
}

const Axis2DInputSource* GameInput::getAxis2DSource (std::string_view sourceId) {
    auto pos = sourceId.find_first_of('.');
    if (pos == std::string_view::npos) {
        PHENYL_LOGE(LOGGER, "Invalid source id: \"{}\"", sourceId);
        return nullptr;
    }

    auto deviceId = sourceId.substr(0, pos);
    auto deviceIt = m_devices.find(deviceId);
    if (deviceIt == m_devices.end()) {
        PHENYL_LOGE(LOGGER, "Unknown device \"{}\" in source \"{}\"", deviceId, sourceId);
        return nullptr;
    }

    return deviceIt->second->getAxis2DSource(sourceId.substr(pos + 1));
}
