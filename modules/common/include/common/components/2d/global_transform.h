#pragma once

#include "common/maths/2d/transform.h"

namespace util {
    class DataValue;
}

namespace common {
    struct GlobalTransform2D {
    private:
    public:
        GlobalTransform2D() = default;
        /*GlobalTransform2D (glm::mat2 _transform) : transform{_transform}, rotTransform{_transform} {}
        //glm::vec2 pos;
        glm::mat2 transform{{1.0f, 0.0f}, {0.0f, 1.0f}};
        glm::mat2 rotTransform;*/
        common::Transform2D transform2D;
    };

    util::DataValue phenyl_to_data (const GlobalTransform2D& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, GlobalTransform2D& comp);
}