#pragma once

#include <phenyl/component.h>
#include <phenyl/serialization.h>

namespace breakout {
    struct Ball {

    };

    PHENYL_SERIALIZE(Ball, {})

    void initBall (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer);
}