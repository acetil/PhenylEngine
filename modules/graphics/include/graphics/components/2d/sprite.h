#pragma once

#include <utility>

#include "graphics/maths_headers.h"
#include "graphics/texture.h"
#include "common/assets/asset.h"

namespace phenyl::graphics {
    struct Sprite2D {
        common::Asset<Texture> texture;

        glm::vec2 uvStart{0, 0};
        glm::vec2 uvEnd{1, 1};
    };
}