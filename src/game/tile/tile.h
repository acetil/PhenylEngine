#include <string>
#include "graphics/textures/texture_atlas.h"
#ifndef TILE_H
#define TILE_H
namespace game {
    class Tile {
        // xSize and ySize relate to normal tile size i.e. a normal tile should be 1.0f, 1.0f
        protected:
        std::string name;
        int textureId;
        float* uvs;
        float* coords;
        public:
        float xSize;
        float ySize;

        Tile (std::string name, int textureId, graphics::TextureAtlas* atlas, float xSize, float ySize);
        virtual ~Tile();
        virtual int getTextureId ();
        virtual std::string getName ();
        virtual float* getVertexCoords (float x, float y);
        virtual float* getUvs ();
        virtual bool shouldDraw ();
    };
}
#endif