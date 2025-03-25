#include "core/maths/3d/transform.h"
#include "core/serialization/serializer_impl.h"

namespace phenyl::core {
    PHENYL_SERIALIZABLE(Transform3D,
        PHENYL_SERIALIZABLE_MEMBER_NAMED(transformPos, "position"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(transformScale, "scale"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(transformRot, "rotation"))
}
