#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util/data_funcs.h"
#include "util/data.h"

namespace util {
    class DataValue;
}

namespace glm {
    bool fromdata (const util::DataValue& val, vec2& v);
    util::DataValue todata (const vec2& v);

    bool fromdata(const util::DataValue& val, vec4& v);
    util::DataValue todata (const vec4& v);
}

namespace util {
    class DataValue;

    template<>
    inline bool fromdata<glm::vec2> (const util::DataValue& val, glm::vec2& v) {
        return glm::fromdata(val, v);
    }

    template<>
    inline util::DataValue todata<glm::vec2> (const glm::vec2& v) {
        return glm::todata(v);
    }

    template<>
    inline bool fromdata<glm::vec4> (const util::DataValue& val, glm::vec4& v) {
        return glm::fromdata(val, v);
    }
    template<>
    inline util::DataValue todata<glm::vec4> (const glm::vec4& v) {
        return glm::todata(v);
    }
}
