#include "graphics/graphics_new_include.h"
using namespace graphics;


util::DataValue Model2D::serialise () const{
    return util::DataValue(modelName);
}

void Model2D::deserialise (util::DataValue const& val) {
    modelName = val.get<std::string>(); // TODO
}

util::DataValue Transform2D::serialise () const {
    util::DataObject obj;
    util::DataArray arr;
    for (int i = 0; i < 2; i++) {
        arr.emplace_back(transform[i]);
    }
    obj["transform"] = arr;
    return (util::DataValue)obj;
}

void Transform2D::deserialise (const util::DataValue& val) {
    auto& obj = val.get<util::DataObject>();
    if (obj.contains("transform")) {
        auto& arr = obj.at("transform").get<util::DataArray>();
        for (int i = 0; i < 2; i++) {
            transform[i] = arr[i].get<glm::vec2>();
        }
    }
    rotTransform = transform;
}