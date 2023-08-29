#pragma once

#include "physics/physics.h"

namespace physics {
    struct Collider2D {
        ShapeId hitbox{};
        //ShapeId eventbox{};

        glm::vec2 currentPos = {0, 0};

        bool shapesMerged = true;

        std::uint64_t hitboxLayers = 0;
        std::uint64_t eventboxMask = 0;

        float mass{100};
        float inertialMoment{100};

        float elasticity{1.0f};

        glm::vec2 momentum;
        float angularMomentum;

        glm::vec2 appliedImpulse{0.0f, 0.0f};
        float appliedAngularImpulse{0.0f};

        component::EntityId entityId{};
        bool updated = false;

        Collider2D () = default;
        Collider2D (component::EntityId entityId) {};

        [[nodiscard]] glm::vec2 getCurrVelocity () const {
            return mass != 0 ? (momentum + appliedImpulse) / mass : glm::vec2{0, 0};
        }

        [[nodiscard]] float getCurrAngularVelocity () const {
            return inertialMoment != 0 ? (angularMomentum + appliedAngularImpulse) / inertialMoment : 0.0f;
        }

        void applyImpulse (glm::vec2 impulse) {
            appliedImpulse += impulse;
        }

        void applyAngularImpulse (float angularImpulse) {
            appliedAngularImpulse += angularImpulse;
        }
    };
}