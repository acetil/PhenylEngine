#include "physics/components/2D/rigid_body.h"
#include "util/data.h"
#include "common/components/2d/global_transform.h"

#define MIN_ANGULAR_VEL 0.01f
#define MAX_ANGULAR_VEL (3.14f * 2.0f)

using namespace physics;

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

util::DataValue RigidBody2D::serialise () const {
    util::DataObject dataObj;
    dataObj["momentum"] = momentum;
    dataObj["angular_momentum"] = angularMomentum;

    dataObj["mass"] = mass;
    dataObj["inertial_moment"] = inertialMoment;

    dataObj["gravity"] = gravity;
    dataObj["drag"] = drag;
    dataObj["angular_drag"] = angularDrag;

    return dataObj;
}

bool RigidBody2D::deserialise (const util::DataValue& dataVal) {
    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& dataObj = dataVal.get<util::DataObject>();

    if (dataObj.contains("momentum")) {
        momentum = dataObj.at("momentum").get<glm::vec2>();
    }

    if (dataObj.contains<float>("angular_momentum")) {
        angularMomentum = dataObj.at("angularMomentum").get<float>();
    }

    if (dataObj.contains<float>("mass")) {
        setMass(dataObj.at("mass").get<float>());
    }

    if (dataObj.contains<float>("inertial_moment")) {
        setInertia(dataObj.at("inertial_moment").get<float>());
    }

    if (dataObj.contains("gravity")) {
        gravity = dataObj.at("gravity").get<glm::vec2>();
    }

    if (dataObj.contains<float>("drag")) {
        drag = dataObj.at<float>("drag");
    }

    if (dataObj.contains<float>("angular_drag")) {
        angularDrag = dataObj.at<float>("angular_drag");
    }

    return true;
}
