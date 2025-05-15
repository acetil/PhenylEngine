#pragma once

#include "graphics/maths_headers.h"
#include "physics/components/2D/collider.h"

namespace phenyl::physics {
struct SATResult2D {
    glm::vec2 normal;
    float depth;

    [[nodiscard]] SATResult2D operator- () const {
        return SATResult2D{.normal{-normal}, .depth = depth};
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

    float lambdaSum{0.0f};

    std::array<float, 2> lambdaClamp;

    static Constraint2D ContactConstraint (Collider2D* obj1, Collider2D* obj2, glm::vec2 contactPoint, glm::vec2 normal,
        float bias);

    bool solve ();
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

    Constraint2D buildConstraint (Collider2D* obj1, Collider2D* obj2, float deltaTime) const;

    glm::vec2 getContactPoint () const;
};

Manifold2D buildManifold (const Face2D& face1, const Face2D& face2, glm::vec2 normal, float depth);
} // namespace phenyl::physics
