#include "image.h"
#include "texture.h"
#include <vector>
#include <unordered_map>
#include <string>
#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
namespace graphics {
    class TextureAtlas {
        private:
            unsigned char* data;
            Texture* textures;
            std::unordered_map<std::string, int> textureIdMap;
        public:
            void createAtlas (std::vector<Image*> images);
            int getTextureId (std::string name);
            Texture* getTexture (int textureId);
            Texture* getTexture (std::string name);
    };
}
#endif