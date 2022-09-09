#include "component/main_component.h"
#include "util/data.h"


util::DataValue component::EntityMainComponent::serialise () const {
    util::DataObject obj;
    //obj["pos"] = pos;
    obj["vel"] = vel;
    obj["acc"] = acc;
    obj["const_friction"] = constFriction;
    obj["lin_friction"] = linFriction;
    return (util::DataValue)obj;
}

void component::EntityMainComponent::deserialise (const util::DataValue& val) {
    auto& obj = val.get<util::DataObject>();
    /*if (obj.contains("pos")) {
        pos = obj.at("pos").get<glm::vec2>();
    }*/
    if (obj.contains("vel")) {
        vel = obj.at("vel").get<glm::vec2>();
    }
    if (obj.contains("acc")) {
        acc = obj.at("acc").get<glm::vec2>();
    }
    if (obj.contains("const_friction")) {
        constFriction = obj.at("const_friction").get<float>();
    }
    if (obj.contains("lin_friction")) {
        linFriction = obj.at("lin_friction").get<float>();
    }
}
