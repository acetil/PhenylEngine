#include "map.h"
#include "game/tile/tile.h"

using namespace game;

game::Map::Map (int width, int height) {
    this->width = width;
    this->height = height;
    tiles = NULL;
}
game::Map::~Map () {
    delete[] tiles;
}
Tile* game::Map::getTile (int x, int y) {
    return tiles[x + width * y];
}
void game::Map::setTiles (Tile** tiles) {
    this->tiles = tiles;
}
Tile** game::Map::getTiles () {
    return tiles;
}
int game::Map::getWidth () {
    return width;
}
int game::Map::getHeight () {
    return height;
}
