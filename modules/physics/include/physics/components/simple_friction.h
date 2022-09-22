#pragma once

#include "graphics/maths_headers.h"

namespace component {
    class Position2D;
}

namespace physics {
    struct KinematicMotion2D;

    struct SimpleFriction {
        float constFriction = 0.0f;
        float linFriction = 0.0f;

        void updateFriction2D (KinematicMotion2D& motion2D) const;
    };

    util::DataValue phenyl_to_data (const SimpleFriction& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, SimpleFriction& comp);
}