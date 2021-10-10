#include "maths_headers.h"

#include "util/data.h"

bool glm::fromdata (const util::DataValue& val, vec2& v) {
    auto& obj = val.get<util::DataObject>();
    if (!(obj.contains("x") && obj.contains("y"))) {
        return false;
    }
    v.x = obj.at("x").get<float>();
    v.y = obj.at("y").get<float>();
    return true;
}
util::DataValue glm::todata (vec2& v) {
    util::DataObject obj;
    obj["x"] = v.x;
    obj["y"] = v.y;
    return (util::DataValue)obj;
}