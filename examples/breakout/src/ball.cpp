#include <phenyl/signals/physics.h>

#include "ball.h"

static constexpr std::uint32_t FLOOR_LAYER = 1 << 2;

using namespace breakout;

void breakout::initBall (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serializer) {
    manager.addComponent<Ball>();
    serializer.addSerializer<Ball>();

    manager.handleSignal<phenyl::signals::OnCollision, Ball>([] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, const Ball&) {
        if (signal.collisionLayers & FLOOR_LAYER) {
            entity.remove();
        }
    });
}