#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/signals/physics.h>

#include "ball.h"

static constexpr std::uint32_t FLOOR_LAYER = 1 << 2;

using namespace breakout;

void breakout::initBall (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer) {
    manager.addComponent<Ball>();
    serializer.addSerializer<Ball>();

    manager.handleSignal<phenyl::signals::OnCollision, const Ball>([] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, const Ball&) {
        if (signal.collisionLayers & FLOOR_LAYER) {
            entity.remove();
        }
    });
}

void breakout::updateBall (phenyl::ComponentManager& manager) {
    manager.query<phenyl::RigidBody2D, const Ball>().each([] (auto entity, phenyl::RigidBody2D& body, const Ball& ball) {
        auto vel = body.getMomentum() * body.getInvMass();

        if (glm::dot(vel, vel) > ball.maxSpeed * ball.maxSpeed) {
            body.applyImpulse(-body.getMomentum() * (glm::length(vel) - ball.maxSpeed) * ball.appliedDrag);
        }
    });
}