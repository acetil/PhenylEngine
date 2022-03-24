#pragma once

#include "image.h"
#include "graphics/graphics_headers.h"
#include "graphics/renderers/renderer.h"

#include <vector>
#include <unordered_map>
#include <string>
#include "graphics/graphics_new_include.h"

namespace graphics {
    class TextureAtlas {
        private:
            //unsigned char* data;
            std::shared_ptr<unsigned char[]> data;
            GraphicsTexture graphicsTexture;
            int sideLength;
            //float* positionData;
            std::shared_ptr<glm::vec2[]> positionData;
            std::shared_ptr<glm::vec2[]> uvData;
            //float* uvData;
            std::vector<FixedModel> models;
            std::unordered_map<std::string, int> modelIdMap;
        public:
            void createAtlas (const std::vector<Model>& modelsIn);
            //int getTextureId (std::string name);
            void loadTextureAtlas (Renderer* renderer);
            //Texture* getTexture (int textureId);
            //Texture* getTexture (std::string name);
            FixedModel getModel (int modelId);

            [[maybe_unused]]
            FixedModel getModel (const std::string& name);
            int getModelId (const std::string& name);
            void bindTextureAtlas ();
            //TextureAtlas(TextureAtlas&) = delete;
            //TextureAtlas& operator=(TextureAtlas&) = delete;
    };
}
