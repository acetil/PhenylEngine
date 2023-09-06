#pragma once

#include "collider.h"
#include "common/serializer.h"

namespace physics {
    PHENYL_SERIALIZE(ColliderComp2D, {
        PHENYL_MEMBER(layers);
        PHENYL_MEMBER(mask);
        PHENYL_MEMBER(elasticity);
    })
}