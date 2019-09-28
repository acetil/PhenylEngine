#include <string>
#include "game/tile/tile.h"
#include "graphics/graphics.h"
#ifndef MAP_H
#define MAP_H
namespace game {
    class Map {
        protected:
        int width;
        int height;
        Tile** tiles;
        graphics::StaticData* graphicsData;
        virtual Tile** getTiles ();
        public:
        Map (int xSize, int ySize);
        ~Map();
        virtual Tile* getTile (int x, int y);
        virtual void setTiles (Tile** tiles);
        virtual int getWidth ();
        virtual int getHeight ();
        virtual void initGraphicsData (graphics::Graphics* graphics, std::string shader);
        virtual void unloadGraphicsData (graphics::Graphics* graphics);
    };
}
#endif