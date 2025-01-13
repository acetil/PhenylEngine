#pragma once

#include "graphics/maths_headers.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::graphics {
    struct PointLight3D {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float brightness = 1.0f;
    };

    PHENYL_DECLARE_SERIALIZABLE(PointLight3D)
}