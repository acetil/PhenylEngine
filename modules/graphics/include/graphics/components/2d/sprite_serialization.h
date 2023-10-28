#pragma once

#include "common/serializer.h"

#include "sprite.h"
#include "common/assets/asset_serializer.h"
#include "common/assets/asset_serializer_f.h"

namespace phenyl::graphics {
    PHENYL_SERIALIZE(Sprite2D, {
        PHENYL_MEMBER(texture);
    })
}