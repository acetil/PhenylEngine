#include "graphics/maths_headers.h"
#ifndef MAIN_COMPONENT_H
#define MAIN_COMPONENT_H
namespace component {
    struct EntityMainComponent {
        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec2 acc;
        float constFriction;
        float linFriction;
    };
}
#endif