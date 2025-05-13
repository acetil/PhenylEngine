#pragma once

#include "core/maths/2d/transform.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
struct GlobalTransform2D {
    GlobalTransform2D () = default;
    /*GlobalTransform2D (glm::mat2 _transform) : transform{_transform}, rotTransform{_transform}
    {}
    //glm::vec2 pos;
    glm::mat2 transform{{1.0f, 0.0f}, {0.0f, 1.0f}};
    glm::mat2 rotTransform;*/
    Transform2D transform2D;
};

PHENYL_DECLARE_SERIALIZABLE(GlobalTransform2D)
} // namespace phenyl::core
