#include "graphics/maths_headers.h"

#include "util/data.h"

using namespace phenyl;

bool glm::phenyl_from_data (const util::DataValue& val, glm::vec2& v) {
    if (val.is<util::DataObject>()) {
        auto& obj = val.get<util::DataObject>();
        if (!(obj.contains("x") && obj.contains("y"))) {
            return false;
        }
        v.x = obj.at("x").get<float>();
        v.y = obj.at("y").get<float>();
        return true;
    } else if (val.is<util::DataArray>()) {
        auto& arr = val.get<util::DataArray>();

        if (arr.size() != 2) {
            return false;
        }
        v.x = arr[0].get<float>();
        v.y = arr[1].get<float>();
        return true;
    }
    return false;
}

bool glm::phenyl_from_data (const util::DataValue& val, glm::vec4& v) {
    if (val.is<util::DataObject>()) {
        auto& obj = val.get<util::DataObject>();
        if (!(obj.contains("x") && obj.contains("y"))) {
            return false;
        }
        v.x = obj.at("x").get<float>();
        v.y = obj.at("y").get<float>();
        v.z = obj.at("z").get<float>();
        v.w = obj.at("w").get<float>();
        return true;
    } else if (val.is<util::DataArray>()) {
        auto& arr = val.get<util::DataArray>();

        if (arr.size() != 4) {
            return false;
        }
        v.x = arr[0].get<float>();
        v.y = arr[1].get<float>();
        v.z = arr[2].get<float>();
        v.w = arr[3].get<float>();
        return true;
    }
    return false;
}

util::DataValue glm::phenyl_to_data (const glm::vec4& v) {
    util::DataArray arr;
    arr[0] = v.x;
    arr[1] = v.y;
    arr[2] = v.z;
    arr[3] = v.w;

    return (util::DataValue)arr;
}

util::DataValue glm::phenyl_to_data (const glm::vec2& v) {
    util::DataObject obj;
    obj["x"] = v.x;
    obj["y"] = v.y;
    return (util::DataValue)obj;
}

bool glm::phenyl_from_data (const util::DataValue& val, glm::mat2& m) {
    if (!val.is<util::DataArray>()) {
        return false;
    }

    const auto& arr = val.get<util::DataArray>();

    if (arr.size() != 2) {
        return false;
    }

    glm::vec2 vec1;
    if (!phenyl_from_data(arr[0], vec1)) {
        return false;
    }

    // For some reason if this is m[1] instead of vec2 the memory of m gets corrupted ????
    glm::vec2 vec2;
    if (!phenyl_from_data(arr[1], vec2)) {
        return false;
    }

    m[0] = vec1;
    m[1] = vec2;

    return true;
}

util::DataValue glm::phenyl_to_data (const glm::mat2& m) {
    util::DataArray arr;
    arr.emplace_back(m[0]);
    arr.emplace_back(m[1]);

    return (util::DataValue)arr;
}