#pragma once

#include "common/serializer.h"
#include "physics/components/2D/collider_serialize.h"
#include "box_collider.h"

namespace phenyl::physics {
    PHENYL_SERIALIZE(BoxCollider2D, {
        PHENYL_INHERITS_NAMED(Collider2D, "Collider2D");
        PHENYL_MEMBER(scale);
    })
}