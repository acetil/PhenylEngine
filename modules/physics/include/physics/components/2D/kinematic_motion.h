#pragma once

#include "graphics/maths_headers.h"

namespace common {
    class GlobalTransform2D;
}
namespace physics {
    struct KinematicMotion2D {
    public:
        glm::vec2 velocity;
        glm::vec2 acceleration;

        void doMotion (common::GlobalTransform2D& transform2D);
    };

    util::DataValue phenyl_to_data (const KinematicMotion2D& motion2D);
    bool phenyl_from_data (const util::DataValue& dataVal, KinematicMotion2D& motion2D);
}