#include "physics/components/simple_friction.h"
#include "physics/components/2D/kinematic_motion.h"
#include "util/data.h"
#include "component/components/2D/position.h"


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

void physics::SimpleFriction::updateFriction2D (physics::KinematicMotion2D& motion2D) const {
    int isPosXVel = motion2D.velocity.x > 0;
    int isPosYVel = motion2D.velocity.y > 0;

    motion2D.velocity.x -= ((float)motion2D.velocity.x * linFriction) + constFriction * (isPosXVel * 2 - 1);
    motion2D.velocity.y -= ((float)motion2D.velocity.y * linFriction) + constFriction * (isPosYVel * 2 - 1);

    motion2D.velocity.x *= (motion2D.velocity.x > 0 && isPosXVel) || (motion2D.velocity.x < 0 && !isPosXVel);
    motion2D.velocity.y *= (motion2D.velocity.y > 0 && isPosYVel) || (motion2D.velocity.y < 0 && !isPosYVel);
}
