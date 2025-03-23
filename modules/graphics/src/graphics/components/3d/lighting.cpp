#include "core/serialization/serializer_impl.h"
#include "graphics/components/3d/lighting.h"

namespace phenyl::graphics {
    PHENYL_SERIALIZABLE(DirectionalLight3D,
        PHENYL_SERIALIZABLE_MEMBER(color),
        PHENYL_SERIALIZABLE_MEMBER(brightness),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(castShadows, "cast_shadows")
    )

    PHENYL_SERIALIZABLE(PointLight3D,
        PHENYL_SERIALIZABLE_MEMBER(color),
        PHENYL_SERIALIZABLE_MEMBER(brightness),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(castShadows, "cast_shadows")
    )

    PHENYL_SERIALIZABLE(SpotLight3D,
        PHENYL_SERIALIZABLE_MEMBER(color),
        PHENYL_SERIALIZABLE_MEMBER(brightness),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(castShadows, "cast_shadows"),
        PHENYL_SERIALIZABLE_MEMBER(outerAngle),
        PHENYL_SERIALIZABLE_MEMBER(innerAngle)
    )
}