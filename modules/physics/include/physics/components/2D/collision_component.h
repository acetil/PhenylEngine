#pragma once

#include "graphics/maths_headers.h"
#include "util/data_funcs.h"

#include "physics/physics.h"

namespace physics {
    struct CollisionComponent2D {
    private:
    public:
        glm::mat2 bbMap{{1.0f, 0.0f}, {0.0f, 1.0f}};
        glm::mat2 rotBBMap{{1.0f, 0.0f}, {0.0f, 1.0f}};
        float outerRadius{1.0f};
        float mass{0.0f};
        unsigned int layers = 0;
        unsigned int masks = 0;
        unsigned int resolveLayers = 0;
        unsigned int eventLayer = 0;
    };

    util::DataValue phenyl_to_data (const CollisionComponent2D& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, CollisionComponent2D& comp);

    struct CollisionComponent2DNew {
    public:
        ColliderId collider{};

        glm::mat2 transform;
    };
}
