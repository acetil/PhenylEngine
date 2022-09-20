#pragma once

#include "graphics/maths_headers.h"

namespace component {
}

namespace physics {
    struct SimpleFrictionMotion2D {
    private:
    public:
        glm::vec2 velocity{};
        glm::vec2 acceleration{};
        float constFriction = 0.0f;
        float linFriction = 0.0f;
    };

    util::DataValue phenyl_to_data (const SimpleFrictionMotion2D& val);
    bool phenyl_from_data (const util::DataValue& dataVal, SimpleFrictionMotion2D& val);
}