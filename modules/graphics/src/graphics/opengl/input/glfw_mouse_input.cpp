#include <vector>

#include "glfw_mouse_input.h"

using namespace graphics;

long GLFWMouseInput::getInputNum (const std::string& inputStr) {
    if (buttonMap.contains(inputStr)) {
        return buttonMap.at(inputStr);
    }

    return -1;
}

bool GLFWMouseInput::isDown (long inputNum) {
    if (consumed.contains(inputNum)) {
        return false;
    }
    return glfwGetMouseButton(window, (int)inputNum) == GLFW_PRESS;
}

void GLFWMouseInput::consume (long inputNum) {
    consumed[inputNum] = true;
}

void GLFWMouseInput::update () {
    std::vector<long> toRemove;
    for (auto [k, v] : consumed.kv()) {
        if (glfwGetMouseButton(window, (int)k) == GLFW_RELEASE) {
            toRemove.push_back(k);
        }
    }

    for (auto& l : toRemove) {
        consumed.remove(l);
    }
}

void GLFWMouseInput::setupButtons () {
    buttonMap["mouse_left"] = GLFW_MOUSE_BUTTON_LEFT;
    buttonMap["mouse_right"] = GLFW_MOUSE_BUTTON_RIGHT;
    buttonMap["mouse_middle"] = GLFW_MOUSE_BUTTON_MIDDLE;
    buttonMap["mouse_button_4"] = GLFW_MOUSE_BUTTON_4;
    buttonMap["mouse_button_5"] = GLFW_MOUSE_BUTTON_5;
    buttonMap["mouse_button_6"] = GLFW_MOUSE_BUTTON_6;
    buttonMap["mouse_button_7"] = GLFW_MOUSE_BUTTON_7;
    buttonMap["mouse_button_8"] = GLFW_MOUSE_BUTTON_8;
}

std::size_t GLFWMouseInput::getStateNum (long inputNum) {
    return 0;
}

void GLFWMouseInput::onMouseButtonChange (int button, int action, int mods) {

}

GLFWMouseInput2::GLFWMouseInput2 () {
    addButton("mouse_left", GLFW_MOUSE_BUTTON_LEFT);
    addButton("mouse_right", GLFW_MOUSE_BUTTON_RIGHT);
    addButton("mouse_middle", GLFW_MOUSE_BUTTON_MIDDLE);
    addButton("mouse_button_4", GLFW_MOUSE_BUTTON_4);
    addButton("mouse_button_5", GLFW_MOUSE_BUTTON_4);
    addButton("mouse_button_6", GLFW_MOUSE_BUTTON_4);
    addButton("mouse_button_7", GLFW_MOUSE_BUTTON_4);
    addButton("mouse_button_8", GLFW_MOUSE_BUTTON_4);
}
