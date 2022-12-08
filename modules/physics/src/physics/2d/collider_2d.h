#pragma once

#include "physics/physics.h"

namespace physics {
    struct Collider2D {
        ShapeId hitbox{};
        ShapeId eventbox{};

        glm::vec2 currentPos = {0, 0};
        glm::vec2 nextPos = {0, 0};

        glm::mat2 transform = {{1, 0}, {0, 1}};

        bool shapesMerged = true;

        std::uint64_t hitboxLayers = 0;
        std::uint64_t eventboxMask = 0;

        component::EntityId entityId{};

        Collider2D ();
        Collider2D (component::EntityId entityId);
    };
}