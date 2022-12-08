#include "physics/components/2D/kinematic_motion.h"
#include "component/components/2D/position.h"
#include "util/data.h"

using namespace physics;

void KinematicMotion2D::doMotion (component::Position2D& pos2D) {
    velocity += acceleration;
    pos2D += velocity;
}

util::DataValue physics::phenyl_to_data (const physics::KinematicMotion2D& motion2D) {
    util::DataObject dataObj;
    dataObj["vel"] = motion2D.velocity;
    dataObj["acc"] = motion2D.acceleration;

    return dataObj;
}

bool physics::phenyl_from_data (const util::DataValue& dataVal, physics::KinematicMotion2D& motion2D) {
    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& dataObj = dataVal.get<util::DataObject>();

    if (dataObj.contains("vel")) {
        motion2D.velocity = dataObj.at("vel").get<glm::vec2>();
    }

    if (dataObj.contains("acc")) {
        motion2D.acceleration = dataObj.at("acc").get<glm::vec2>();
    }

    return true;
}