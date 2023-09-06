#pragma once

#include "transform.h"
#include "common/serializer.h"
#include "common/maths/glm_serializer.h"

namespace common {
    PHENYL_SERIALIZE(Transform2D, {
        PHENYL_MEMBER_NAMED(positionVec, "position");
        PHENYL_MEMBER_METHOD("rotation", rotationAngle, setRotation);
        PHENYL_MEMBER_NAMED(scaleVec, "scale");
    })
}