#pragma once

#include "common/serializer.h"
#include "common/maths/2d/transform_serializer.h"
#include "global_transform.h"

namespace phenyl::common {
    PHENYL_SERIALIZE(GlobalTransform2D, {
        PHENYL_MEMBER_NAMED(transform2D, "transform");
    })
}