#ifndef MATHS_HEADERS_H
#define MATHS_HEADERS_H
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util/data_funcs.h"
namespace glm {
    bool fromdata (const util::DataValue& val, vec2& v);
    util::DataValue todata (vec2& v);
}
#endif