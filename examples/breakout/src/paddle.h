#pragma once

#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/serialization.h>

namespace breakout {
    struct Paddle {
        bool hasBall = true;
        float speed = 1.0f;
    };

    PHENYL_SERIALIZE(Paddle, {
        PHENYL_MEMBER_NAMED(hasBall, "has_ball");
        PHENYL_MEMBER(speed);
    })

    void initPaddle (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer, phenyl::GameInput& input);

    void updatePaddle (float deltaTime, phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera);
}