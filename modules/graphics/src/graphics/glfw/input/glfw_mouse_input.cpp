#include "graphics/detail/loggers.h"
#include "logging/logging.h"

#include "glfw_mouse_input.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GLFW_MOUSE_INPUT", detail::GRAPHICS_LOGGER};

GLFWMouseInput::GLFWMouseInput (GLFWwindow* window) : window{window} {
    buttonIds["button_left"] = GLFW_MOUSE_BUTTON_LEFT;
    buttonIds["button_right"] = GLFW_MOUSE_BUTTON_RIGHT;
    buttonIds["button_middle"] = GLFW_MOUSE_BUTTON_MIDDLE;
    buttonIds["button_4"] = GLFW_MOUSE_BUTTON_4;
    buttonIds["button_5"] = GLFW_MOUSE_BUTTON_5;
    buttonIds["button_6"] = GLFW_MOUSE_BUTTON_6;
    buttonIds["button_7"] = GLFW_MOUSE_BUTTON_7;
    buttonIds["button_8"] = GLFW_MOUSE_BUTTON_8;
}

const phenyl::common::ButtonInputSource* GLFWMouseInput::getButtonSource (std::string_view sourcePath) {
    auto idIt = buttonIds.find(sourcePath);
    if (idIt == buttonIds.end()) {
        PHENYL_LOGE(LOGGER, "Invalid button: \"{}\"", sourcePath);
        return nullptr;
    }

    auto sourceIt = sources.find(idIt->second);
    if (sourceIt != sources.end()) {
        return &sourceIt->second;
    }

    auto it = sources.emplace(idIt->second, common::ButtonInputSource{}).first;
    return &it->second;
}

const phenyl::common::Axis2DInputSource* GLFWMouseInput::getAxis2DSource (std::string_view sourcePath) {
    if (sourcePath != "mouse_pos") {
        PHENYL_LOGE(LOGGER, "Invalid Axis2D source: \"{}\"", sourcePath);
        return nullptr;
    }

    return &mousePosSource;
}

std::string_view GLFWMouseInput::getDeviceId () const noexcept {
    return "mouse";
}

void GLFWMouseInput::poll () {
    for (auto& [id, source] : sources) {
        source.setState(glfwGetMouseButton(window, id) == GLFW_PRESS);
    }

    double cursorX;
    double cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    mousePosSource.setState(glm::vec2{cursorX, cursorY});
}
