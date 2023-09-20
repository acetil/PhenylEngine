#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::graphics::ui {
    enum class Axis {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    glm::vec2 getAxisVec (Axis axis);
}