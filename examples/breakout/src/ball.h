#pragma once

#include <phenyl/component.h>
#include <phenyl/serialization.h>

namespace breakout {
    struct Ball {
        float maxSpeed = 1.0f;
        float appliedDrag = 0.3;
    };

    PHENYL_SERIALIZE(Ball, {
        PHENYL_MEMBER_NAMED(maxSpeed, "max_speed");
        PHENYL_MEMBER_NAMED(appliedDrag, "applied_drag");
    })

    void initBall (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer);
    void updateBall (phenyl::ComponentManager& manager);
}