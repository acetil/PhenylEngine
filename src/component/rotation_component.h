#include "graphics/maths_headers.h"
#ifndef ROTATION_COMPONENT_H
#define ROTATION_COMPONENT_H
namespace component {
    struct RotationComponent {
        glm::mat2 rotMatrix;
        float rotation;
    };
}
#endif
