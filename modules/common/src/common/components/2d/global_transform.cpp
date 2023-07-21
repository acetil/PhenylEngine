#include "common/components/2d/global_transform.h"
#include "util/data.h"

bool common::phenyl_from_data (const util::DataValue& dataVal, common::GlobalTransform2D& comp) {
    /* if (!dataVal.is<util::DataObject>()) {
         return false;
     }

     auto& obj = dataVal.get<util::DataObject>();
     if (obj.contains("transform")) {
         auto& arr = obj.at("transform").get<util::DataArray>();
         for (int i = 0; i < 2; i++) {
             comp.transform[i] = arr[i].get<glm::vec2>();
         }
     }
     comp.rotTransform = comp.transform;

     return true;*/

    if (!dataVal.is<util::DataObject>()) {
        return false;
    }

    const auto& obj = dataVal.get<util::DataObject>();
    if (!obj.contains("transform")) {
        return false;
    }

    const auto& transformVal = obj.at("transform");

    return phenyl_from_data(transformVal, comp.transform2D);
}

util::DataValue common::phenyl_to_data (const common::GlobalTransform2D& comp) {
    /*util::DataObject obj;
    util::DataArray arr;
    for (int i = 0; i < 2; i++) {
        arr.emplace_back(comp.transform[i]);
    }
    obj["transform"] = std::move(arr);
    return (util::DataValue)obj;*/

    util::DataObject obj;
    obj["transform"] = comp.transform2D;

    return obj;
}