#pragma once

#include "graphics/maths_headers.h"
#include "physics/2d/collider_2d.h"

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

    struct Constraint2D {
        Collider2D* obj1;
        Collider2D* obj2;
        glm::vec2 jVelObj1;
        glm::vec2 jVelObj2;
        float jWObj1;
        float jWObj2;

        float invJacobMass;
        float bias;

        float lambdaSum;

        std::array<float, 2> lambdaClamp;

        Constraint2D (Collider2D* obj1, Collider2D* obj2, glm::vec2 contactPoint, glm::vec2 normal, float bias, std::array<float, 2> lambdaClamp);

        bool solve (float deltaTime);
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

        void buildConstraints (std::vector<Constraint2D>& constraints, Collider2D* obj1, Collider2D* obj2, float deltaTime) const;
    };

    Manifold2D buildManifold (const Face2D& face1, const Face2D& face2, glm::vec2 normal, float depth);
}