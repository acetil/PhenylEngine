#pragma once

#include "physics/physics.h"
#include "graphics/maths_headers.h"

namespace physics {
    struct Collider2D {
        ShapeId hitbox{};
        glm::vec2 currentPos = {0, 0};

        std::uint64_t hitboxLayers = 0;
        std::uint64_t eventboxMask = 0;

        float invMass{1/100.0f};
        float invInertiaMoment{1/100.0f};

        float elasticity{0.0f};

        glm::vec2 momentum{0.0f};
        float angularMomentum{0.0f};

        glm::vec2 appliedImpulse{0.0f, 0.0f};
        float appliedAngularImpulse{0.0f};

        component::EntityId entityId{};
        bool updated = false;

        Collider2D () = default;
        Collider2D (component::EntityId entityId): entityId{entityId} {};

        [[nodiscard]] glm::vec2 getCurrVelocity () const {
            return (momentum + appliedImpulse) * invMass;
        }

        [[nodiscard]] float getCurrAngularVelocity () const {
            return (angularMomentum + appliedAngularImpulse) * invInertiaMoment;
        }

        void applyImpulse (glm::vec2 impulse) {
            appliedImpulse += impulse;
        }

        void applyAngularImpulse (float angularImpulse) {
            appliedAngularImpulse += angularImpulse;
        }
    };
}