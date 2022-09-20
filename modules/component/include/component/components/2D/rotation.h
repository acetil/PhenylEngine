#pragma once

#include "graphics/maths_headers.h"
#include "util/data_funcs.h"

namespace component {
    struct Rotation2D {
    private:
    public:
        glm::mat2 rotMatrix;
        float rotation;
        Rotation2D& operator= (float newRot);
    };

    util::DataValue phenyl_to_data (const Rotation2D& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, Rotation2D& comp);
}
