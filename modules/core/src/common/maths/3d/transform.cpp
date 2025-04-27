#include "core/maths/3d/transform.h"
#include "core/serialization/serializer_impl.h"

namespace phenyl::core {
    PHENYL_SERIALIZABLE(Transform3D,
        PHENYL_SERIALIZABLE_MEMBER_NAMED(m_position, "position"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(m_scale, "scale"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(m_rotation, "rotation"))
}
