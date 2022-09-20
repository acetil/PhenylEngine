#include "physics/components/2D/simple_friction.h"
#include "util/data.h"


util::DataValue physics::phenyl_to_data (const physics::SimpleFrictionMotion2D& val) {
    util::DataObject obj;
    obj["velocity"] = val.velocity;
    obj["acceleration"] = val.acceleration;
    obj["const_friction"] = val.constFriction;
    obj["lin_friction"] = val.linFriction;
    return obj;
}

bool physics::phenyl_from_data (const util::DataValue& dataVal, physics::SimpleFrictionMotion2D& val) {
    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& obj = dataVal.get<util::DataObject>();

    physics::SimpleFrictionMotion2D newVal{};

    if (obj.contains("velocity")) {
        newVal.velocity = obj.at("velocity").get<glm::vec2>();
    }

    if (obj.contains("acceleration")) {
        newVal.acceleration = obj.at("acceleration").get<glm::vec2>();
    }

    if (obj.contains("const_friction")) {
        newVal.constFriction = obj.at("const_friction").get<float>();
    }

    if (obj.contains("lin_friction")) {
        newVal.linFriction = obj.at("lin_friction").get<float>();
    }

    val = newVal;

    return true;
}
