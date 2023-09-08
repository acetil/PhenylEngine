#pragma once

#include "common/serializer.h"
#include "graphics_new_include.h"

namespace graphics {
    PHENYL_SERIALIZE(Model2D, {
        PHENYL_MEMBER_NAMED(modelName, "model_name");
    })
}