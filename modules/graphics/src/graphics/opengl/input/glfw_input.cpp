#include "glfw_input.h"

#include "graphics/graphics_headers.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GLFW_INPUT"};

namespace phenyl::graphics::detail {
    struct ButtonState {
        std::size_t stateNum{};
        int code;
        int mods{0};
        bool currentState{false};
        bool isEnabled{true};
        bool hasChanged{false};
        explicit ButtonState (int _code) : code{_code} {}

        void setState (bool _currentState, int _mods) {
            currentState = _currentState;
            mods = _mods;
            isEnabled = true;
            hasChanged = true;
            stateNum++;
        }
    };
}

long GLFWInput::getInputNum (const std::string& inputStr) {
    if (buttonNameMap.contains(inputStr)) {
        return buttonNameMap.at(inputStr);
    }
    return -1;
}

bool GLFWInput::isDown (long inputNum) {
    if (inputNum < 0 || inputNum >= buttons.size()) {
        PHENYL_LOGE(LOGGER, "Unable to find button for input num {}!", inputNum);
    }
    return buttons[inputNum].isEnabled && buttons[inputNum].currentState;
}

void GLFWInput::consume (long inputNum) {
    if (inputNum < 0 || inputNum >= buttons.size()) {
        PHENYL_LOGE(LOGGER, "Unable to find button for input num {}!", inputNum);
    }
    buttons[inputNum].isEnabled = false;
}

std::size_t GLFWInput::getStateNum (long inputNum) {
    if (inputNum < 0 || inputNum >= buttons.size()) {
        PHENYL_LOGE(LOGGER, "Unable to find button for input num {}!", inputNum);
    }
    return buttons[inputNum].stateNum;
}

void GLFWInput::addButton (const std::string& button, int buttonCode) {
    buttonNameMap[button] = (long)buttons.size();
    buttonCodeMap[buttonCode] = (long)buttons.size();

    buttons.emplace_back(buttonCode);
}

void GLFWInput::onButtonChange (int code, int action, int mods) {
    if (!buttonCodeMap.contains(code)) {
        PHENYL_LOGE(LOGGER, "Unable to find button with code {}!", code);
        return;
    }
    long button = buttonCodeMap[code];

    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        buttons[button].setState(action == GLFW_PRESS, mods);
    }
}


GLFWInput::GLFWInput () = default;
GLFWInput::~GLFWInput () = default;


