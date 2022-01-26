#pragma once

#include <string>
#include "graphics/textures/texture_atlas.h"

namespace game {
    class Tile {
        // xSize and ySize relate to normal tile size i.e. a normal tile should be 1.0f, 1.0f
        // TODO: move from virtuals
        protected:
        std::string name;
        int textureId;
        public:
        float xSize;
        float ySize;

        Tile (std::string name, int textureId, graphics::TextureAtlas& atlas, float xSize, float ySize);
        virtual ~Tile();
        virtual int getTextureId ();
        virtual std::string getName ();
        //virtual float* getVertexCoords (float x, float y);
        //virtual float* getUvs ();
        virtual bool shouldDraw ();
        virtual int getModelId ();
    };
}