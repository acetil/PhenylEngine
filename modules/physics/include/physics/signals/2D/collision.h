#pragma once

#include <cstdint>

#include "component/entity_id.h"

namespace physics {
    struct OnCollision {
        component::EntityId otherId;
        std::uint32_t collisionLayers;
    };
}