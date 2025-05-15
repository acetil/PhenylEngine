#pragma once

#include "core/assets/asset.h"
#include "core/serialization/serializer_forward.h"
#include "graphics/backend/texture.h"
#include "graphics/maths_headers.h"

#include <utility>

namespace phenyl::graphics {
struct Sprite2D {
    core::Asset<Texture> texture;

    glm::vec2 uvStart{0, 0};
    glm::vec2 uvEnd{1, 1};
};

PHENYL_DECLARE_SERIALIZABLE(Sprite2D)
} // namespace phenyl::graphics
