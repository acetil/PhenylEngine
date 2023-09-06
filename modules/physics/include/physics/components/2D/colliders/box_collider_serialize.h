#pragma once

#include "common/serializer.h"
#include "physics/components/2D/collider_serialize.h"
#include "box_collider.h"

namespace physics {
    PHENYL_SERIALIZE(BoxCollider2D, {
        PHENYL_INHERITS_NAMED(ColliderComp2D, "Collider2D");
        PHENYL_MEMBER(scale);
    })
}