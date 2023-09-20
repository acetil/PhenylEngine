#pragma once

#include "collider.h"
#include "common/serializer.h"

namespace phenyl::physics {
    PHENYL_SERIALIZE(Collider2D, {
        PHENYL_MEMBER(layers);
        PHENYL_MEMBER(mask);
        PHENYL_MEMBER(elasticity);
    })
}