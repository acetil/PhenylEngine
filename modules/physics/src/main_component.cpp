#include "component/main_component.h"
#include "util/data.h"


util::DataValue component::FrictionKinematicsMotion2D::serialise () const {
    util::DataObject obj;
    //obj["pos"] = pos;
    obj["velocity"] = velocity;
    obj["acceleration"] = acceleration;
    obj["const_friction"] = constFriction;
    obj["lin_friction"] = linFriction;
    return (util::DataValue)obj;
}

void component::FrictionKinematicsMotion2D::deserialise (const util::DataValue& val) {
    auto& obj = val.get<util::DataObject>();
    /*if (obj.contains("pos")) {
        pos = obj.at("pos").get<glm::vec2>();
    }*/
    if (obj.contains("velocity")) {
        velocity = obj.at("velocity").get<glm::vec2>();
    }
    if (obj.contains("acceleration")) {
        acceleration = obj.at("acceleration").get<glm::vec2>();
    }
    if (obj.contains("const_friction")) {
        constFriction = obj.at("const_friction").get<float>();
    }
    if (obj.contains("lin_friction")) {
        linFriction = obj.at("lin_friction").get<float>();
    }
}
