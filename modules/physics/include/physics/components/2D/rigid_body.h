#pragma once

#include "core/serialization/serializer_impl.h"
#include "graphics/maths_headers.h"
#include "physics/physics.h"

namespace phenyl::core {
class GlobalTransform2D;
}

namespace phenyl::physics {
struct RigidBody2D {
public:
    glm::vec2 gravity{0, 0};

    float drag{0.0f};
    float angularDrag{0.0f};

    [[nodiscard]] float mass () const {
        return m_mass;
    }

    [[nodiscard]] float invMass () const {
        return m_invMass;
    }

    void setMass (float newMass) {
        m_mass = newMass;
        m_invMass = newMass != 0 ? 1 / newMass : 0.0f;
    }

    [[nodiscard]] float inertia () const {
        return m_inertialMoment;
    }

    [[nodiscard]] float invInertia () const {
        return m_invInertialMoment;
    }

    void setInertia (float inertia) {
        m_inertialMoment = inertia;
        m_invInertialMoment = inertia != 0 ? 1 / inertia : 0.0f;
    }

    void doMotion (core::GlobalTransform2D& transform2D, float deltaTime);

    void applyForce (glm::vec2 force);
    void applyForce (glm::vec2 force, glm::vec2 worldDisplacement);

    void applyImpulse (glm::vec2 impulse);
    void applyImpulse (glm::vec2 impulse, glm::vec2 worldDisplacement);

    void applyAngularImpulse (float angularImpulse);
    void applyTorque (float torque);

    [[nodiscard]] const glm::vec2& momentum () const {
        return m_momentum;
    }

    [[nodiscard]] float angularMomentum () const {
        return m_angularMomentum;
    }

private:
    glm::vec2 m_momentum{0, 0};
    glm::vec2 m_netForce{0, 0};

    float m_angularMomentum{0.0f};
    float m_torque{0.0f};

    float m_mass{1.0f};
    float m_invMass{1.0f};
    float m_inertialMoment{1.0f};
    float m_invInertialMoment{1.0f};

    void applyFriction ();

    PHENYL_SERIALIZABLE_INTRUSIVE (RigidBody2D);
};
} // namespace phenyl::physics
