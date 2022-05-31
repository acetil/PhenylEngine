#pragma once

#include <string>

#include "common/input/input_source.h"
#include "util/map.h"
#include "util/set.h"
#include "graphics/graphics_headers.h"
#include "glfw_input.h"

namespace graphics {
    class GLFWKeyInput : public common::InputSource {
    private:
        util::Map<std::string, long> keyMap{};
        util::Set<long> consumed{};
        util::Map<long, bool> lastInputStates{};
        util::Map<long, std::size_t> stateNums{};
        GLFWwindow* window;

        void setupKeys ();
    public:
        explicit GLFWKeyInput (GLFWwindow* _window) : window{_window} {
            setupKeys();
        };

        long getInputNum(const std::string &inputStr) override;
        bool isDown(long inputNum) override;
        void consume(long inputNum) override;

        std::size_t getStateNum(long inputNum) override;

        void update ();
    };

    class GLFWKeyInput2 : public GLFWInput {
    public:
        GLFWKeyInput2 ();
    };
}