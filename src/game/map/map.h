#include <string>
#include "game/tile/tile.h"
#ifndef MAP_H
#define MAP_H
namespace game {
    class Map {
        protected:
        int width;
        int height;
        Tile** tiles;
        //graphics::StaticData* graphicsData;
        virtual Tile** getTiles ();
        graphics::TextureAtlas atlas;
        public:
        Map (int xSize, int ySize);
        ~Map();
        virtual Tile* getTile (int x, int y);
        virtual void setTiles (Tile** tiles);
        virtual int getWidth ();
        virtual int getHeight ();
        //virtual void initGraphicsData (graphics::Graphics* graphics, std::string shader);
        //virtual void unloadGraphicsData (graphics::Graphics* graphics);

        //virtual int getNumTileVertices (); // TODO: move to models
        //virtual float* getTileVertices ();
        //virtual float* getTileUvs ();
        virtual std::vector<std::pair<graphics::AbsolutePosition, graphics::FixedModel>> getModels ();
        virtual void setAtlas (graphics::TextureAtlas atlas);
    };
}
#endif