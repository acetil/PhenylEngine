#pragma once

#include "graphics/maths_headers.h"

namespace physics {
    struct SATResult2D {
        glm::vec2 normal;
        float depth;

        [[nodiscard]] SATResult2D operator- () const {
            return SATResult2D{.normal{-normal}, .depth=depth};
        }

        [[nodiscard]] inline glm::vec2 separationVec () const {
            return normal * depth;
        }
    };
}