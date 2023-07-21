#pragma once

#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
#include <utility>
#include <optional>

#include "graphics/shaders/shaders.h"
#include "graphics/textures/image.h"
#include "logging/logging.h"
#include "camera.h"
#include "util/span.h"
#include "common/maths/2d/transform.h"

//#include "buffer.h"
//#include "renderer.h"

namespace graphics {
    struct TextureOffset {
        glm::vec2 offset;
        glm::mat2 recMat;
        TextureOffset (glm::vec2 off, glm::mat2 mat) : offset{off}, recMat{mat} {};
        TextureOffset () : TextureOffset ({0.0f, 0.0f}, {{1.0f, 0.0f}, {0.0f, 1.0f}}) {};
    };


    struct Model {
        std::string modelName;
        std::string texPath;
        std::vector<std::pair<TextureOffset, Image::SharedPtr>> textures;
        Model (std::string modelName, Image::SharedPtr image) {
            this->modelName = std::move(modelName);
            textures.emplace_back(TextureOffset(), image);
            texPath = "";
        }
    };

    struct Model2D {
    private:
    public:
        util::span<glm::vec2> positionData;
        util::span<glm::vec2> uvData;
        std::string modelName;
        Model2D () : positionData(), uvData() {}
        Model2D (glm::vec2* posPtr, glm::vec2* uvPtr, int size, std::string name) {
            positionData = util::span(posPtr, size / 2);
            uvData = util::span(uvPtr, size / 2);
            modelName = std::move(name);
        }
        Model2D (glm::vec2* posStart, glm::vec2* posEnd, glm::vec2* uvStart, glm::vec2 * uvEnd, std::string name) {
            /*logging::logf(LEVEL_DEBUG, "Pos size: %ld", posEnd - posStart);
            logging::logf(LEVEL_DEBUG, "UV size: %ld", uvEnd - uvStart);*/
#ifndef NDEBUG
            assert(posEnd - posStart == uvEnd - uvStart);
#endif
            positionData = util::span(posStart, posEnd);
            uvData = util::span(uvStart, uvEnd);
            modelName = std::move(name);
        }
    };

    util::DataValue phenyl_to_data (const Model2D& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, Model2D& comp);

    /*struct GlobalTransform2D {
    private:
    public:
        GlobalTransform2D() = default;
        GlobalTransform2D (glm::mat2 _transform) : transform{_transform}, rotTransform{_transform} {}
        //glm::vec2 pos;
        glm::mat2 transform{{1.0f, 0.0f}, {0.0f, 1.0f}};
        glm::mat2 rotTransform;
        common::Transform2D transform2D;
    };

    util::DataValue phenyl_to_data (const GlobalTransform2D& comp);
    bool phenyl_from_data (const util::DataValue& dataVal, GlobalTransform2D& comp);*/

}
