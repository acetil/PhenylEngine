#include "graphics/maths_headers.h"
#ifndef COLLISION_COMPONENT_H
#define COLLISION_COMPONENT_H
namespace physics {
    struct CollisionComponent {
        glm::vec2 pos;
        glm::mat2 bbMap;
        float outerRadius;
        float mass;
        unsigned int layers = 0;
        unsigned int masks = 0;
        unsigned int resolveLayers = 0;
        unsigned int eventLayer = 0;
    };
}
#endif //COLLISION_COMPONENT_H
