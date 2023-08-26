#include "physics/components/simple_friction.h"
#include "physics/components/2D/rigid_body.h"
#include "util/data.h"


util::DataValue physics::phenyl_to_data (const physics::SimpleFriction& comp) {
    util::DataObject obj;
    obj["const_friction"] = comp.constFriction;
    obj["lin_friction"] = comp.linFriction;

    return obj;
}

bool physics::phenyl_from_data (const util::DataValue& dataVal, SimpleFriction& comp) {
    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& dataObj = dataVal.get<util::DataObject>();

    if (dataObj.contains("const_friction")) {
        comp.constFriction = dataObj.at("const_friction").get<float>();
    }

    if (dataObj.contains("lin_friction")) {
        comp.linFriction = dataObj.at("lin_friction").get<float>();
    }

    return true;
}

void physics::SimpleFriction::updateFriction2D (physics::RigidBody2D& motion2D, float deltaTime) const {
    /*int isPosXVel = motion2D.momentum.x > 0;
    int isPosYVel = motion2D.momentum.y > 0;

    motion2D.momentum.x -= ((float)motion2D.momentum.x * linFriction) + constFriction * (isPosXVel * 2 - 1);
    motion2D.momentum.y -= ((float)motion2D.momentum.y * linFriction) + constFriction * (isPosYVel * 2 - 1);

    motion2D.momentum.x *= (motion2D.momentum.x > 0 && isPosXVel) || (motion2D.momentum.x < 0 && !isPosXVel);
    motion2D.momentum.y *= (motion2D.momentum.y > 0 && isPosYVel) || (motion2D.momentum.y < 0 && !isPosYVel);*/
    motion2D.applyFriction(*this, deltaTime);
}
