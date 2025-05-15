#pragma once

#include "core/entity_id.h"
#include "graphics/maths_headers.h"

#include <cstdint>

namespace phenyl::physics {
struct OnCollision {
    core::EntityId otherId;
    std::uint32_t collisionLayers;
    glm::vec2 worldContactPoint;
    glm::vec2 normal;
};
} // namespace phenyl::physics
