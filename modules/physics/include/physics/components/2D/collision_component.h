#pragma once

#include "graphics/maths_headers.h"
#include "util/data_funcs.h"

#include "physics/physics.h"

namespace physics {
    struct CollisionComponent2D {
    public:
        ColliderId collider{};

        glm::mat2 transform;
    };
}
