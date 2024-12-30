#include "core/serialization/serializer_impl.h"

#include "core/components/2d/global_transform.h"
#include "core/components/3d/global_transform.h"

namespace phenyl::core {
    PHENYL_SERIALIZABLE(GlobalTransform2D, PHENYL_SERIALIZABLE_MEMBER_NAMED(transform2D, "transform"))

    PHENYL_SERIALIZABLE(GlobalTransform3D, PHENYL_SERIALIZABLE_MEMBER(transform))
}
