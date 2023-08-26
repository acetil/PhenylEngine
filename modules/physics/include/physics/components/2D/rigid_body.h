#pragma once

#include "graphics/maths_headers.h"

namespace common {
    class GlobalTransform2D;
}
namespace physics {
    class SimpleFriction;

    struct RigidBody2D {
    private:
        glm::vec2 momentum{0, 0};
        glm::vec2 netForce{0, 0};

        // TODO
        friend util::DataValue phenyl_to_data (const RigidBody2D& motion2D);
        friend bool phenyl_from_data (const util::DataValue& dataVal, RigidBody2D& motion2D);
    public:
        float mass{1.0f};
        glm::vec2 gravity{0, 0};

        void doMotion (common::GlobalTransform2D& transform2D, float deltaTime);
        void applyForce (glm::vec2 force);
        void applyImpulse (glm::vec2 impulse);

        void applyFriction (const SimpleFriction& friction, float deltaTime);
    };

    util::DataValue phenyl_to_data (const RigidBody2D& motion2D);
    bool phenyl_from_data (const util::DataValue& dataVal, RigidBody2D& motion2D);
}