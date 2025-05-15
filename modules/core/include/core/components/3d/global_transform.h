#pragma once

#include "core/maths/3d/transform.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
struct GlobalTransform3D {
    Transform3D transform{};
};

PHENYL_DECLARE_SERIALIZABLE(GlobalTransform3D)
} // namespace phenyl::core
