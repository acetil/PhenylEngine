#pragma once

#include "graphics/maths_headers.h"
#include "shape_2d.h"

namespace physics {
    struct BoxShape2D : public Shape2D {
        glm::mat2 localTransform;
        glm::mat2 transform;

        explicit BoxShape2D (glm::mat2 localSizeTransform, std::uint64_t layers, std::uint64_t mask);

        void applyWorldTransform (glm::mat2 worldTransform);
        void setLocalTransform (glm::mat2 localTransform);
    };

    util::Optional<CollisionResponse2D> collideShape2D (const BoxShape2D& shape1, const ShapeVariant2D& shape2, glm::vec2 disp);
    util::Optional<CollisionResponse2D> collideShape2D (const BoxShape2D& shape1, const BoxShape2D& shape2, glm::vec2 disp);
}