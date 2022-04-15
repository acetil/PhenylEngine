#include "graphics/graphics_update.h"
using namespace graphics;

void graphics::updateEntityRotation (event::EntityRotationEvent &event) {
    auto ptr = event.manager->getObjectDataPtr<AbsolutePosition>(event.entityId).orElse(nullptr);
    //ptr->transform *= event.rotMatrix;
}

util::DataValue FixedModel::serialise () const{
    return util::DataValue(modelName);
}

void FixedModel::deserialise (util::DataValue const& val) {
    modelName = val.get<std::string>(); // TODO
}

util::DataValue AbsolutePosition::serialise () const {
    util::DataObject obj;
    util::DataArray arr;
    for (int i = 0; i < 2; i++) {
        arr.emplace_back(transform[i]);
    }
    obj["transform"] = arr;
    return (util::DataValue)obj;
}

void AbsolutePosition::deserialise (const util::DataValue& val) {
    auto& obj = val.get<util::DataObject>();
    if (obj.contains("transform")) {
        auto& arr = obj.at("transform").get<util::DataArray>();
        for (int i = 0; i < 2; i++) {
            transform[i] = arr[i].get<glm::vec2>();
        }
    }
    rotTransform = transform;
}