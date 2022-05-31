#pragma once

#include "common/input/input_source.h"

#include "graphics/graphics_headers.h"

#include "util/map.h"
#include "glfw_input.h"

namespace graphics {
    struct ButtonState {
        std::size_t stateNum;
        int scancode;
        bool currentState;
        bool isEnabled;
        bool hasChanged;
    };
    class GLFWMouseInput : public common::InputSource {
    private:
        util::Map<std::string, long> buttonMap{};
        util::Map<long, bool> consumed{}; // TODO: set?
        std::vector<ButtonState> buttonStates;
        GLFWwindow* window;
        void setupButtons ();
    public:
        explicit GLFWMouseInput (GLFWwindow* _window) : window{_window} {
            setupButtons();
        }

        long getInputNum(const std::string &inputStr) override;
        bool isDown(long inputNum) override;
        void consume(long inputNum) override;

        void update ();

        std::size_t getStateNum(long inputNum) override;

        void onMouseButtonChange (int buttons, int action, int mods);
    };

    class GLFWMouseInput2 : public GLFWInput {
    protected:
    public:
        GLFWMouseInput2();
    };
}