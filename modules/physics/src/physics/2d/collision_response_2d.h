#pragma once

#include "physics/physics.h"

namespace physics {
    struct CollisionResponse2D {
        glm::vec2 overlap;

        CollisionResponse2D operator- () const {
            return {-overlap};
        }
    };
}