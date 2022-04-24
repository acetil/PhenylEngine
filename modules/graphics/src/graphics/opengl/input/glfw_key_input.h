#pragma once

#include <string>

#include "common/input/input_source.h"
#include "util/map.h"
#include "graphics/graphics_headers.h"

namespace graphics {
    class GLFWKeyInput : public common::InputSource {
    private:
        util::Map<std::string, long> keyMap{};
        util::Map<long, bool> consumed{}; // TODO: set?
        GLFWwindow* window;

        void setupKeys ();
    public:
        explicit GLFWKeyInput (GLFWwindow* _window) : window{_window} {
            setupKeys();
        };

        long getInputNum(const std::string &inputStr) override;
        bool isDown(long inputNum) override;
        void consume(long inputNum) override;

        void update ();
    };
}