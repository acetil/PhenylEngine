#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace phenyl::util {
    class DataValue;
}

namespace glm {
    bool phenyl_from_data (const phenyl::util::DataValue& val, vec2& v);
    phenyl::util::DataValue phenyl_to_data (const vec2& v);

    bool phenyl_from_data(const phenyl::util::DataValue& val, vec4& v);
    phenyl::util::DataValue phenyl_to_data (const vec4& v);

    bool phenyl_from_data (const phenyl::util::DataValue& val, glm::mat2& m);
    phenyl::util::DataValue phenyl_to_data (const glm::mat2& m);
}