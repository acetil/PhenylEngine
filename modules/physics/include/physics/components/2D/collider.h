#pragma once

#include "core/serialization/serializer_forward.h"
#include "graphics/maths_headers.h"

namespace phenyl::physics {
class RigidBody2D;

class Collider2D {
public:
    glm::vec2 currentPos = {0, 0};
    std::uint64_t layers = 0;
    std::uint64_t mask = 0;
    float elasticity{0.0f};

    Collider2D () = default;

    void syncUpdates (const RigidBody2D& body, glm::vec2 pos);
    void updateBody (RigidBody2D& body) const;
    [[nodiscard]] bool shouldCollide (const Collider2D& other) const;

protected:
    void setOuterRadius (float newOuterRadius) {
        m_outerRadius = newOuterRadius;
    }

    [[nodiscard]] glm::vec2 getPosition () const {
        return currentPos;
    }

    [[nodiscard]] glm::vec2 getDisplacement (const Collider2D& other) const {
        return other.currentPos - currentPos;
    }

private:
    float m_invMass{1.0f};
    float m_invInertiaMoment{1.0f};

    glm::vec2 m_momentum{0.0f};
    float m_angularMomentum{0.0f};

    glm::vec2 m_appliedImpulse{0.0f, 0.0f};
    float m_appliedAngularImpulse{0.0f};

    float m_outerRadius{0.0f};

    [[nodiscard]] glm::vec2 getCurrVelocity () const {
        return (m_momentum + m_appliedImpulse) * m_invMass;
    }

    [[nodiscard]] float getCurrAngularVelocity () const {
        return (m_angularMomentum + m_appliedAngularImpulse) * m_invInertiaMoment;
    }

    void applyImpulse (glm::vec2 impulse) {
        m_appliedImpulse += impulse;
    }

    void applyAngularImpulse (float angularImpulse) {
        m_appliedAngularImpulse += angularImpulse;
    }

    friend class Physics2D;
    friend class Constraint2D;
    friend class Manifold2D;
};

PHENYL_DECLARE_SERIALIZABLE(Collider2D)
} // namespace phenyl::physics
