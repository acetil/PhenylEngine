#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    struct CanvasRect {
        glm::vec2 size{};
    };

    struct CanvasRoundedRect {
        CanvasRect rect{};
        float rounding = 1.0f;
    };
}