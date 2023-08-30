#pragma once

#include "graphics/maths_headers.h"
#include "physics/physics.h"

namespace common {
    class GlobalTransform2D;
}
namespace physics {
    struct RigidBody2D {
    private:
        ColliderId colliderId;
        glm::vec2 momentum{0, 0};
        glm::vec2 netForce{0, 0};

        float angularMomentum{0.0f};
        float torque{0.0f};

        float mass{1.0f};
        float invMass{1.0f};
        float inertialMoment{1.0f};
        float invInertialMoment{1.0f};

        // TODO
        friend util::DataValue phenyl_to_data (const RigidBody2D& body);
        friend bool phenyl_from_data (const util::DataValue& dataVal, RigidBody2D& body);

        void applyFriction ();
    public:
        explicit RigidBody2D (ColliderId collider) : colliderId{collider} {}
        glm::vec2 gravity{0, 0};
        //float elasticity{0.0f};

        float drag{0.0f};
        float angularDrag{0.0f};

        [[nodiscard]] float getMass () const {
            return mass;
        }
        [[nodiscard]] float getInvMass () const {
            return invMass;
        }
        void setMass (float newMass) {
            mass = newMass;
            invMass = newMass != 0 ? 1 / newMass : 0.0f;
        }

        [[nodiscard]] float getInertia () const {
            return inertialMoment;
        }
        [[nodiscard]] float getInvInertia () const {
            return invInertialMoment;
        }
        void setInertia (float inertia) {
            inertialMoment = inertia;
            invInertialMoment = inertia != 0 ? 1 / inertia : 0.0f;
        }

        void doMotion (common::GlobalTransform2D& transform2D, float deltaTime);

        void applyForce (glm::vec2 force);
        void applyForce (glm::vec2 force, glm::vec2 worldDisplacement);

        void applyImpulse (glm::vec2 impulse);
        void applyImpulse (glm::vec2 impulse, glm::vec2 worldDisplacement);

        void applyAngularImpulse (float angularImpulse);
        void applyTorque (float torque);

        [[nodiscard]] const glm::vec2& getMomentum () const {
            return momentum;
        }

        [[nodiscard]] float getAngularMomentum () const {
            return angularMomentum;
        }

        ColliderId getCollider () const {
            return colliderId;
        };
    };

    util::DataValue phenyl_to_data (const RigidBody2D& body);
    bool phenyl_from_data (const util::DataValue& dataVal, RigidBody2D& body);
}