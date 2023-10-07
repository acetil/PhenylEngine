#pragma once

#include <cstdint>

#include "graphics/maths_headers.h"
#include "component/entity_id.h"

namespace phenyl::physics {
    struct OnCollision {
        component::EntityId otherId;
        std::uint32_t collisionLayers;
        glm::vec2 worldContactPoint;
        glm::vec2 normal;
    };
}