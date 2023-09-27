#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>

#include "paddle.h"

using namespace breakout;

static phenyl::InputAction LeftKey;
static phenyl::InputAction RightKey;

void breakout::initPaddle (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer, phenyl::GameInput& input) {
    manager.addComponent<Paddle>();
    serializer.addSerializer<Paddle>();

    LeftKey = input.mapInput("move_left", "key_a");
    RightKey = input.mapInput("move_right", "key_d");
}

void breakout::updatePaddle (float deltaTime, phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera) {
    manager.query<phenyl::GlobalTransform2D, const Paddle>().each([&input, &camera, deltaTime] (auto entity, phenyl::GlobalTransform2D& transform, const Paddle& paddle) {
        glm::vec2 direction{};
        if (input.isDown(LeftKey)) {
            direction += glm::vec2{-1.0f, 0.0f};
        }

        if (input.isDown(RightKey)) {
            direction += glm::vec2{1.0f, 0.0f};
        }

        transform.transform2D.translate(direction * paddle.speed * deltaTime);
    });
}