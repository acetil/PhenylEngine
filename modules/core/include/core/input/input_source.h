#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::core {
    class ButtonInputSource {
    private:
        bool currState = false;
    public:
        [[nodiscard]] bool state () const noexcept {
            return currState;
        }

        void setState (bool newState) noexcept {
            currState = newState;
        }
    };

    class Axis2DInputSource {
    private:
        glm::vec2 axis{};
    public:
        [[nodiscard]] glm::vec2 state () const noexcept {
            return axis;
        }

        void setState (glm::vec2 newAxis) noexcept {
            axis = newAxis;
        }
    };
}