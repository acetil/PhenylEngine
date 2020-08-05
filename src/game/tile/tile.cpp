#include <string>
#include "tile.h"
#include "graphics/textures/texture_atlas.h"

using namespace game;
#define VERTICES_PER_TRIANGLE 3
#define TRIANGLES_PER_TILE 2
#define NUM_POS_PER_VERTEX 2
game::Tile::Tile (std::string name, int textureId, graphics::TextureAtlas& atlas, float xSize, float ySize) {
    this->name = name;
    this->textureId = textureId;
    //this->uvs = atlas.getTexture(textureId)->getTexUvs();
    this->coords = new float[VERTICES_PER_TRIANGLE * TRIANGLES_PER_TILE * NUM_POS_PER_VERTEX];
    this->xSize = xSize;
    this->ySize = ySize;
}
game::Tile::~Tile () {
    delete[] coords;
}
int game::Tile::getTextureId () {
    return textureId;
}
std::string game::Tile::getName () {
    return name;
}
/*float* game::Tile::getVertexCoords (float x, float y) {
    float* coordsPtr = coords;
    // from graphics.cpp
    // TODO: refactor all
    for (int i = 0; i < VERTICES_PER_TRIANGLE * TRIANGLES_PER_TILE; i++) {
        int correctedVertex = i % VERTICES_PER_TRIANGLE + i / VERTICES_PER_TRIANGLE;
        float vertexX = x;
        float vertexY = y;
        if (correctedVertex < 2) {
            vertexY += ySize / 2;
        } else {
            vertexY -= ySize  / 2;
        }
        if (correctedVertex % 2 == 0) {
            vertexX -= xSize / 2;
        } else {
            vertexX += xSize / 2;
        }
        *(coordsPtr++) = vertexX;
        *(coordsPtr++) = vertexY;
    }
    return coords;
}
float* game::Tile::getUvs () {
    return uvs;
}*/
bool game::Tile::shouldDraw () {
    return true; // default is true
}

int Tile::getModelId () {
    return textureId;
}
