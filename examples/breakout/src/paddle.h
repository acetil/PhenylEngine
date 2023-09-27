#pragma once

#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/serialization.h>

namespace breakout {
    struct Paddle {
        bool hasBall = true;
        float speed = 1.0f;
        float width = 1.0f;
        float minX = 0.0f;
        float maxX = 1.0f;
    };

    PHENYL_SERIALIZE(Paddle, {
        PHENYL_MEMBER_NAMED(hasBall, "has_ball");
        PHENYL_MEMBER(speed);
        PHENYL_MEMBER(width);
        PHENYL_MEMBER_NAMED(minX, "min_x");
        PHENYL_MEMBER_NAMED(maxX, "max_x");
    })

    void initPaddle (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer, phenyl::GameInput& input);

    void updatePaddle (float deltaTime, phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera);
}