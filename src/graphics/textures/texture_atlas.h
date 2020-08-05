#include "image.h"
#include "graphics/graphics_headers.h"
#include "graphics/renderer.h"

#include <vector>
#include <unordered_map>
#include <string>
#include "graphics/graphics_new_include.h"

#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
namespace graphics {
    class TextureAtlas {
        private:
            unsigned char* data;
            GraphicsTexture graphicsTexture;
            int sideLength;
            float* positionData;
            float* uvData;
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
    };
}
#endif