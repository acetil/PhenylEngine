#include "physics/components/2D/collider.h"
#include "physics/components/2D/rigid_body.h"

using namespace phenyl;

namespace phenyl::physics {
    PHENYL_SERIALIZABLE(Collider2D,
        PHENYL_SERIALIZABLE_MEMBER(layers),
        PHENYL_SERIALIZABLE_MEMBER(mask),
        PHENYL_SERIALIZABLE_MEMBER(elasticity))
}

bool physics::Collider2D::shouldCollide (const physics::Collider2D& other) const {
    if (!(layers & other.mask || other.layers & mask)) {
        return false;
    }

    auto displacement = getDisplacement(other);
    float sqDispLen = glm::dot(displacement, displacement);
    float radiusLen = m_outerRadius + other.m_outerRadius;

    return sqDispLen < radiusLen * radiusLen;
}

void physics::Collider2D::syncUpdates (const RigidBody2D& body, glm::vec2 pos) {
    currentPos = pos;
    m_invMass = body.invMass();
    m_invInertiaMoment = body.invInertia();
    m_momentum = body.momentum();
    m_angularMomentum = body.angularMomentum();

    m_appliedImpulse = {0.0f, 0.0f};
    m_appliedAngularImpulse = 0.0f;
}

void physics::Collider2D::updateBody (physics::RigidBody2D& body) const {
    body.applyImpulse(m_appliedImpulse);
    body.applyAngularImpulse(m_appliedAngularImpulse);
}
