#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace util {
    class DataValue;
}

namespace glm {
    bool phenyl_from_data (const util::DataValue& val, vec2& v);
    util::DataValue phenyl_to_data (const vec2& v);

    bool phenyl_from_data(const util::DataValue& val, vec4& v);
    util::DataValue phenyl_to_data (const vec4& v);
}