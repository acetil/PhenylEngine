#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::core {
    class ButtonInputSource {
    private:
        bool m_currState = false;
    public:
        [[nodiscard]] bool state () const noexcept {
            return m_currState;
        }

        void setState (bool newState) noexcept {
            m_currState = newState;
        }
    };

    class Axis2DInputSource {
    private:
        glm::vec2 m_axis{};
    public:
        [[nodiscard]] glm::vec2 state () const noexcept {
            return m_axis;
        }

        void setState (glm::vec2 newAxis) noexcept {
            m_axis = newAxis;
        }
    };
}