#include "glfw_mouse_input.h"

#include "logging/logging.h"

using namespace phenyl::glfw;

static phenyl::Logger LOGGER{"GLFW_MOUSE_INPUT", phenyl::graphics::detail::GRAPHICS_LOGGER};

GLFWMouseInput::GLFWMouseInput (GLFWwindow* window) : m_window{window} {
    m_buttonIds["button_left"] = GLFW_MOUSE_BUTTON_LEFT;
    m_buttonIds["button_right"] = GLFW_MOUSE_BUTTON_RIGHT;
    m_buttonIds["button_middle"] = GLFW_MOUSE_BUTTON_MIDDLE;
    m_buttonIds["button_4"] = GLFW_MOUSE_BUTTON_4;
    m_buttonIds["button_5"] = GLFW_MOUSE_BUTTON_5;
    m_buttonIds["button_6"] = GLFW_MOUSE_BUTTON_6;
    m_buttonIds["button_7"] = GLFW_MOUSE_BUTTON_7;
    m_buttonIds["button_8"] = GLFW_MOUSE_BUTTON_8;
}

const phenyl::core::ButtonInputSource* GLFWMouseInput::getButtonSource (std::string_view sourcePath) {
    auto idIt = m_buttonIds.find(sourcePath);
    if (idIt == m_buttonIds.end()) {
        PHENYL_LOGE(LOGGER, "Invalid button: \"{}\"", sourcePath);
        return nullptr;
    }

    auto sourceIt = m_sources.find(idIt->second);
    if (sourceIt != m_sources.end()) {
        return &sourceIt->second;
    }

    auto it = m_sources.emplace(idIt->second, core::ButtonInputSource{}).first;
    return &it->second;
}

const phenyl::core::Axis2DInputSource* GLFWMouseInput::getAxis2DSource (std::string_view sourcePath) {
    if (sourcePath != "mouse_pos") {
        PHENYL_LOGE(LOGGER, "Invalid Axis2D source: \"{}\"", sourcePath);
        return nullptr;
    }

    return &m_mousePosSource;
}

std::string_view GLFWMouseInput::getDeviceId () const noexcept {
    return "mouse";
}

void GLFWMouseInput::poll () {
    for (auto& [id, source] : m_sources) {
        source.setState(glfwGetMouseButton(m_window, id) == GLFW_PRESS);
    }

    double cursorX;
    double cursorY;
    glfwGetCursorPos(m_window, &cursorX, &cursorY);
    m_mousePosSource.setState(glm::vec2{cursorX, cursorY});
}
