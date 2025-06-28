#pragma once

#include "core/maths/3d/transform.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
template <typename... Args>
struct Bundle;

struct GlobalTransform3D {
    glm::mat4 transform = glm::identity<glm::mat4>();

    [[nodiscard]] glm::vec3 position () const noexcept {
        return glm::vec3{transform * glm::vec4{0, 0, 0, 1}};
    }

    Quaternion rotation () const;

    static void PropagateTransforms (const Bundle<const Transform3D, GlobalTransform3D>* parent,
        const Bundle<const Transform3D, GlobalTransform3D>& child);
};

PHENYL_DECLARE_SERIALIZABLE(GlobalTransform3D)
} // namespace phenyl::core
