#include "physics/components/2D/rigid_body.h"
#include "util/data.h"
#include "common/components/2d/global_transform.h"

#define MIN_ANGULAR_VEL 0.01f
#define MAX_ANGULAR_VEL (3.14f * 2.0f)

using namespace phenyl::physics;

inline float vec2dCross (glm::vec2 vec1, glm::vec2 vec2) {
    return vec1.x * vec2.y - vec1.y * vec2.x;
}

void RigidBody2D::doMotion (common::GlobalTransform2D& transform2D, float deltaTime) {
    applyFriction();
    netForce += gravity * mass;

    momentum += netForce * 0.5f * deltaTime;
    transform2D.transform2D.translate(momentum * invMass * deltaTime);
    momentum += netForce * 0.5f * deltaTime;
    netForce = {0, 0};

    angularMomentum = glm::clamp(angularMomentum + torque * 0.5f * deltaTime, -MAX_ANGULAR_VEL * mass, MAX_ANGULAR_VEL * mass);
    transform2D.transform2D.rotateBy(angularMomentum * invInertialMoment);
    angularMomentum = angularMomentum + torque * 0.5f * deltaTime; // Will be clamped before rotation next step

    if (glm::abs(angularMomentum * invInertialMoment) < MIN_ANGULAR_VEL) {
        angularMomentum = 0.0f;
    }

    torque = 0.0f;
}

void RigidBody2D::applyForce (glm::vec2 force) {
    netForce += force;
}

void RigidBody2D::applyForce (glm::vec2 force, glm::vec2 worldDisplacement) {
    netForce += force;

    torque += vec2dCross(worldDisplacement, force);
}

void RigidBody2D::applyImpulse (glm::vec2 impulse) {
    momentum += impulse;
}

void RigidBody2D::applyImpulse (glm::vec2 impulse, glm::vec2 worldDisplacement) {
    momentum += impulse;

    angularMomentum += vec2dCross(worldDisplacement, impulse);
}

void RigidBody2D::applyFriction () {
    applyForce(-drag * momentum);
    applyTorque(-angularMomentum * drag);
}

void RigidBody2D::applyAngularImpulse (float angularImpulse) {
    angularMomentum += angularImpulse;
}

void RigidBody2D::applyTorque (float appliedTorque) {
    torque += appliedTorque;
}
