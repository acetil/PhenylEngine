#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <optional>

#include "graphics/shaders/shaders.h"
#include "textures/texture.h"
#include "textures/image.h"
#include "logging/logging.h"
#include "camera.h"
//#include "buffer.h"
//#include "renderer.h"
#ifndef GRAPHICS_NEW_INCLUDE_H
#define GRAPHICS_NEW_INCLUDE_H
namespace graphics {
    struct TextureOffset {
        float x;
        float y;

        TextureOffset (float x, float y) {
            this->x = x;
            this->y = y;
        }
    };

    struct Model {
        std::string modelName;
        std::string texPath;
        std::vector<std::pair<TextureOffset, Image*>> textures;
        Model (std::string modelName, Image* image) {
            this->modelName = std::move(modelName);
            textures.emplace_back(TextureOffset(0.0f, 0.0f), image);
            texPath = "";
        }
    };

}
#endif //GRAPHICS_NEW_INCLUDE_H
