#include "physics/components/2D/rigid_body.h"

#include "core/components/2d/global_transform.h"

#define MIN_ANGULAR_VEL 0.01f
#define MAX_ANGULAR_VEL (3.14f * 2.0f)

using namespace phenyl::physics;

namespace phenyl::physics {
PHENYL_SERIALIZABLE(RigidBody2D, PHENYL_SERIALIZABLE_MEMBER_NAMED(m_momentum, "momentum"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(m_angularMomentum, "angular_momentum"),
    PHENYL_SERIALIZABLE_METHOD("mass", &RigidBody2D::mass, &RigidBody2D::setMass),
    PHENYL_SERIALIZABLE_METHOD("inertial_moment", &RigidBody2D::inertia, &RigidBody2D::setInertia),
    PHENYL_SERIALIZABLE_MEMBER(drag), PHENYL_SERIALIZABLE_MEMBER_NAMED(angularDrag, "angular_drag"),
    PHENYL_SERIALIZABLE_MEMBER(gravity))
}

inline float vec2dCross (glm::vec2 vec1, glm::vec2 vec2) {
    return vec1.x * vec2.y - vec1.y * vec2.x;
}

void RigidBody2D::doMotion (core::GlobalTransform2D& transform2D, float deltaTime) {
    applyFriction();
    m_netForce += gravity * m_mass;

    m_momentum += m_netForce * 0.5f * deltaTime;
    transform2D.transform2D.translate(m_momentum * m_invMass * deltaTime);
    m_momentum += m_netForce * 0.5f * deltaTime;
    m_netForce = {0, 0};

    m_angularMomentum = glm::clamp(m_angularMomentum + m_torque * 0.5f * deltaTime, -MAX_ANGULAR_VEL * m_mass,
        MAX_ANGULAR_VEL * m_mass);
    transform2D.transform2D.rotateBy(m_angularMomentum * m_invInertialMoment);
    m_angularMomentum = m_angularMomentum + m_torque * 0.5f * deltaTime; // Will be clamped before rotation next step

    if (glm::abs(m_angularMomentum * m_invInertialMoment) < MIN_ANGULAR_VEL) {
        m_angularMomentum = 0.0f;
    }

    m_torque = 0.0f;
}

void RigidBody2D::applyForce (glm::vec2 force) {
    m_netForce += force;
}

void RigidBody2D::applyForce (glm::vec2 force, glm::vec2 worldDisplacement) {
    m_netForce += force;

    m_torque += vec2dCross(worldDisplacement, force);
}

void RigidBody2D::applyImpulse (glm::vec2 impulse) {
    m_momentum += impulse;
}

void RigidBody2D::applyImpulse (glm::vec2 impulse, glm::vec2 worldDisplacement) {
    m_momentum += impulse;

    m_angularMomentum += vec2dCross(worldDisplacement, impulse);
}

void RigidBody2D::applyFriction () {
    applyForce(-drag * m_momentum);
    applyTorque(-m_angularMomentum * drag);
}

void RigidBody2D::applyAngularImpulse (float angularImpulse) {
    m_angularMomentum += angularImpulse;
}

void RigidBody2D::applyTorque (float appliedTorque) {
    m_torque += appliedTorque;
}
