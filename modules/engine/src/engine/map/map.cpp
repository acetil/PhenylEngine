#include <string.h>
#include <string>
#include <utility>
#include <fstream>

#include "engine/map/map.h"
#include "engine/tile/tile.h"
#include "logging/logging.h"
#include "util/data.h"
#include "graphics/textures/texture_atlas.h"

#define VERTICES_PER_TILE 6
#define VCOMP_PER_VERTEX 2
#define UV_PER_VERTEX 2
using namespace game;

game::Map::Map (int width, int height) {
    this->width = width;
    this->height = height;
    tiles = nullptr;
}
game::Map::~Map () {
    delete[] tiles;
}
Tile* game::Map::getTile (int x, int y) {
    return tiles[x + width * y];
}
// TODO: convert to vector and convert from pointers
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

std::vector<std::tuple<glm::vec2, graphics::Transform2D, graphics::Model2D>> Map::getModels () {
    std::vector<std::tuple<glm::vec2, graphics::Transform2D, graphics::Model2D>> models;
    for (int i = 0; i < width * height; i++) {
        if (tiles[i]->shouldDraw()) {
            auto model = atlas->getModel(tiles[i]->getModelId());
            auto transform = graphics::Transform2D{static_cast<int>(model.positionData.size()),
                                                   glm::mat2{{tiles[i]->xSize / 2, 0}, {0, tiles[i]->ySize / 2}}};
            models.emplace_back(glm::vec2{i % width * tiles[i]->xSize, i / width * tiles[i]->ySize}, transform, model);
        }
    }
    return models;
}

void Map::setAtlas (graphics::TextureAtlas& _atlas) {
    this->atlas = &_atlas;
}

void Map::writeMapJson (const std::string& path, util::DataValue entitiesVal) {
    util::DataObject mapData;
    util::DataObject dims;
    dims["width"] = width;
    dims["height"] = height;
    mapData["dimensions"] = std::move(dims);
    int tileIndex = 0;
    std::unordered_map<Tile*, int> tileIdMap;
    util::DataArray tileIds;
    util::DataArray tileArray;
    for (auto i = 0; i < width * height; i++) {
        if (!tileIdMap.contains(tiles[i])) {
            util::DataObject tileObj;
            tileObj["id"] = tileIndex;
            tileObj["tile"] = tiles[i]->getName();
            tileIds.push_back(util::DataObject(tileObj));
            tileIdMap[tiles[i]] = tileIndex++;
        }
        tileArray.push_back((int)tileIdMap[tiles[i]]);
    }
    util::DataArray entityArray;
    if (entitiesVal.empty()) {
        for (auto& entity : entities) {
            util::DataObject entityObj;
            //entityObj["type"] = entity.entityType;
            //util::DataObject pos;
            //pos["x"] = entity.x;
            //pos["y"] = entity.y;
            //entityObj["pos"] = std::move(pos);
            //entityObj["rotation"] = entity.rotation;
            //entityObj["data"] = entity.data;
            entityArray.push_back(entity.data);
        }
    } else {
        entityArray = entitiesVal.get<util::DataArray>();
    }
    mapData["tile_ids"] = std::move(tileIds);
    mapData["tiles"] = std::move(tileArray);
    mapData["entities"] = std::move(entityArray);

    std::ofstream file(path);
    if (file) {
        file << util::DataValue(std::move(mapData)).convertToJsonPretty();
    }
}

/*void game::Map::initGraphicsData (graphics::Graphics* graphics, std::string shader) {
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
}*/

/*int Map::getNumTileVertices () {
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
}*/
