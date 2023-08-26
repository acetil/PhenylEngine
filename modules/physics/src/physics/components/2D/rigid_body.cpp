#include "physics/components/2D/rigid_body.h"
#include "util/data.h"
#include "common/components/2d/global_transform.h"
#include "physics/components/simple_friction.h"

using namespace physics;

void RigidBody2D::doMotion (common::GlobalTransform2D& transform2D) {
    netForce += gravity;
    momentum += netForce;
    transform2D.transform2D.translate(momentum / mass);
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

void RigidBody2D::applyFriction (const SimpleFriction& friction) {
    bool isXPositive = momentum.x >= 0;
    bool isYPositive = momentum.y >= 0;

    momentum.x -= (momentum.x * friction.linFriction) + (isXPositive ? friction.constFriction : -friction.constFriction);
    momentum.y -= (momentum.y * friction.linFriction) + (isYPositive ? friction.constFriction : -friction.constFriction);

    if ((momentum.x < 0 && isXPositive) || (momentum.x >= 0 && !isXPositive)) {
        momentum.x = 0;
    }

    if ((momentum.y < 0 && isYPositive) || (momentum.y >= 0 && !isYPositive)) {
        momentum.y = 0;
    }
}
