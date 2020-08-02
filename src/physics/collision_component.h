#include "graphics/maths_headers.h"
#ifndef COLLISION_COMPONENT_H
#define COLLISION_COMPONENT_H
namespace physics {
    struct CollisionComponent {
        glm::vec2 pos;
        glm::mat2 bbMap;
        float outerRadius;
        unsigned int layers;
    };
}
#endif //COLLISION_COMPONENT_H
