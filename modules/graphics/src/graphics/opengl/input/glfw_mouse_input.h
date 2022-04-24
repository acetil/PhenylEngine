#pragma once

#include "common/input/input_source.h"

#include "graphics/graphics_headers.h"

#include "util/map.h"

namespace graphics {
    class GLFWMouseInput : public common::InputSource {
    private:
        util::Map<std::string, long> buttonMap{};
        util::Map<long, bool> consumed{}; // TODO: set?
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
    };
}