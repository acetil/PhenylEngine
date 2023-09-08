#pragma once

#include "common/serializer.h"
#include "graphics/maths_headers.h"

namespace glm {
    PHENYL_SERIALIZE(vec2, {
        PHENYL_MEMBER(x);
        PHENYL_MEMBER(y);
    })

    PHENYL_SERIALIZE(vec4, {
        PHENYL_MEMBER(x);
        PHENYL_MEMBER(y);
        PHENYL_MEMBER(z);
        PHENYL_MEMBER(a);
    })

    PHENYL_SERIALIZE_ARRAY_FUNC(mat2, [] (auto& mat) {return &mat[0];}, 2)
}