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

    struct Face2D {
        glm::vec2 vertices[2];
        glm::vec2 normal;
    };

    enum class Manifold2DType : char {
        POINT = 1,
        LINE = 2
    };

    struct Manifold2D {
        glm::vec2 points[2];
        glm::vec2 normal;
        float depth;
        Manifold2DType type;
    };

    Manifold2D buildManifold (const Face2D& face1, const Face2D& face2, glm::vec2 normal, float depth);
}