#include "physics/components/2D/rigid_body.h"
#include "util/data.h"
#include "common/components/2d/global_transform.h"
#include "physics/components/simple_friction.h"

#define LIN_FRICTION_MULT 20.0f

using namespace physics;

void RigidBody2D::doMotion (common::GlobalTransform2D& transform2D, float deltaTime) {
    netForce += gravity;

    momentum += netForce * 0.5f * deltaTime;
    transform2D.transform2D.translate(momentum / mass * deltaTime);
    momentum += netForce * 0.5f * deltaTime;

    netForce = {0, 0};
}

void RigidBody2D::applyForce (glm::vec2 force) {
    netForce += force;
}

void RigidBody2D::applyImpulse (glm::vec2 impulse) {
    momentum += impulse;
}


util::DataValue physics::phenyl_to_data (const physics::RigidBody2D& motion2D) {
    util::DataObject dataObj;
    dataObj["momentum"] = motion2D.momentum;
    dataObj["force"] = motion2D.netForce;
    dataObj["mass"] = motion2D.mass;
    dataObj["gravity"] = motion2D.gravity;

    return dataObj;
}

bool physics::phenyl_from_data (const util::DataValue& dataVal, physics::RigidBody2D& motion2D) {
    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& dataObj = dataVal.get<util::DataObject>();

    if (dataObj.contains("momentum")) {
        motion2D.momentum = dataObj.at("momentum").get<glm::vec2>();
    }

    if (dataObj.contains("force")) {
        motion2D.netForce = dataObj.at("force").get<glm::vec2>();
    }

    if (dataObj.contains("mass")) {
        motion2D.mass = dataObj.at("mass").get<float>();
    }

    if (dataObj.contains("gravity")) {
        motion2D.gravity = dataObj.at("gravity").get<glm::vec2>();
    }

    return true;
}

void RigidBody2D::applyFriction (const SimpleFriction& friction, float deltaTime) {
    /*glm::vec2 frictionForce{0, 0};

    bool isXPositive = momentum.x >= 0;
    bool isYPositive = momentum.y >= 0;

    //momentum.x -= (momentum.x * friction.linFriction) + (isXPositive ? friction.constFriction : -friction.constFriction);
    frictionForce.x = momentum.x * friction.linFriction + (isXPositive ? friction.constFriction : -friction.constFriction);
    //momentum.y -= (momentum.y * friction.linFriction) + (isYPositive ? friction.constFriction : -friction.constFriction);
    frictionForce.y = (momentum.y * friction.linFriction) + (isYPositive ? friction.constFriction : -friction.constFriction);

    if (isXPositive && (frictionForce.x * deltaTime + momentum.x) < 0) {
        momentum.x = 0;
    }

    if ((momentum.y < 0 && isYPositive) || (momentum.y >= 0 && !isYPositive)) {
        momentum.y = 0;
    }*/
    auto vel = momentum / mass;
    auto velDir = glm::vec2{vel.x >= 0 ? 1 : -1, vel.y >= 0 ? 1 : -1};
    applyForce(-friction.linFriction * LIN_FRICTION_MULT * vel * vel * velDir);

    // Friction required to go to 0 momentum = (-p / dt) - F
    auto frictionForce = glm::clamp(-momentum / deltaTime - netForce, -glm::vec2{friction.constFriction, friction.constFriction}, glm::vec2{friction.constFriction, friction.constFriction});

    applyForce(frictionForce);
}
