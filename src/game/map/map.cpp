#include <string.h>
#include <string>

#include "map.h"
#include "graphics/graphics.h"
#include "game/tile/tile.h"

#define VERTICES_PER_TILE 6
#define VCOMP_PER_VERTEX 2
#define UV_PER_VERTEX 2
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
void game::Map::initGraphicsData (graphics::Graphics* graphics, std::string shader) {
    int numVertices = 0;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            numVertices += VERTICES_PER_TILE;
        }
    }
    logging::logf(LEVEL_DEBUG, "There are %d vertices in the map.", numVertices);
    float* vertexData = new float[numVertices * VCOMP_PER_VERTEX];
    float* uvData = new float[numVertices * UV_PER_VERTEX];
    float* vertexPtr = vertexData;
    float* uvPtr = uvData;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            logging::logf(LEVEL_DEBUG, "Copying buffers, id = %d. Ptr: %d, %d", i, vertexPtr - vertexData, uvPtr - uvData);
            memcpy(vertexPtr, tiles[i]->getVertexCoords(i % width * tiles[i]->xSize, i / width * tiles[i]->ySize), VERTICES_PER_TILE * VCOMP_PER_VERTEX * sizeof(float));
            memcpy(uvPtr, tiles[i]->getUvs(), VERTICES_PER_TILE * UV_PER_VERTEX * sizeof(float));
            vertexPtr += VERTICES_PER_TILE * VCOMP_PER_VERTEX;
            uvPtr += VERTICES_PER_TILE * UV_PER_VERTEX;
        }
    }
    graphicsData = graphics->loadStaticData(vertexData, uvData, numVertices, numVertices * VCOMP_PER_VERTEX, 
        numVertices * UV_PER_VERTEX, shader);
    
    delete[] vertexData;
    delete[] uvData;
}
void game::Map::unloadGraphicsData (graphics::Graphics* graphics) {
    graphics->unloadStaticData(graphicsData);
    graphicsData = NULL;
}

int Map::getNumTileVertices () {
    int numVertices = 0;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            numVertices += VERTICES_PER_TILE;
        }
    }
    return numVertices;
}

float* Map::getTileVertices () {
    auto* data = new float[getNumTileVertices() * VCOMP_PER_VERTEX];
    float* dataPtr = data;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            float* vertexCoords = tiles[i]->getVertexCoords(i % width * tiles[i]->xSize, i / width * tiles[i]->ySize);
            memcpy(dataPtr, vertexCoords, VERTICES_PER_TILE * VCOMP_PER_VERTEX * sizeof(float));
            dataPtr += VERTICES_PER_TILE * VCOMP_PER_VERTEX;
        }
    }
    logging::log(LEVEL_DEBUG, "Got tile vertices!");
    return data;
}
float* Map::getTileUvs () {
    auto* data = new float[getNumTileVertices() * VCOMP_PER_VERTEX];
    float* dataPtr = data;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            float* uvCoords = tiles[i]->getUvs();
            memcpy(dataPtr, uvCoords, VERTICES_PER_TILE * UV_PER_VERTEX * sizeof(float));
            dataPtr += VERTICES_PER_TILE * VCOMP_PER_VERTEX;
        }
    }
    return data;
}