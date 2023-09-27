#pragma once

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>
#include <phenyl/serialization.h>

namespace breakout {
    struct Paddle {
        bool hasBall = true;
        float speed = 1.0f;
        float width = 1.0f;
        float minX = 0.0f;
        float maxX = 1.0f;
        float ballSpeed = 1.0f;
        phenyl::Asset<phenyl::Prefab> ballPrefab;

        void update (float deltaTime, phenyl::Entity entity, const phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::GameInput& input, phenyl::GameCamera& camera);
    };

    PHENYL_SERIALIZE(Paddle, {
        PHENYL_MEMBER_NAMED(hasBall, "has_ball");
        PHENYL_MEMBER(speed);
        PHENYL_MEMBER(width);
        PHENYL_MEMBER_NAMED(minX, "min_x");
        PHENYL_MEMBER_NAMED(maxX, "max_x");
        PHENYL_MEMBER_NAMED(ballPrefab, "ball_prefab");
        PHENYL_MEMBER_NAMED(ballSpeed, "ball_speed");
    })

    void initPaddle (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer, phenyl::GameInput& input);

    void updatePaddle (float deltaTime, phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera);
}