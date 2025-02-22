#pragma once

#include "graphics/maths_headers.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::graphics {
    struct DirectionalLight3D {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float brightness = 1.0f;
    };
    struct PointLight3D {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float brightness = 1.0f;
    };
    struct SpotLight3D {
        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        float brightness = 1.0f;
        float innerAngle = 30.0f / 180.0f * std::numbers::pi;
        float outerAngle = 60.0f / 180.0f * std::numbers::pi;
    };

    PHENYL_DECLARE_SERIALIZABLE(DirectionalLight3D)
    PHENYL_DECLARE_SERIALIZABLE(PointLight3D)
    PHENYL_DECLARE_SERIALIZABLE(SpotLight3D)
}