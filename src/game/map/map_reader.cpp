#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "map_reader.h"
#include "map.h"

#include "game/tile/tile.h"
#include "logging/logging.h"

#include "util/string_help.h"

#define MAGIC_LEN 4
#define DIMENSION_SIZE 4
#define TILE_TYPE_NUM_SIZE 4

#define STRING_BUFF_SIZE 256

#define EMPTY_TILE_NUM 0
#define EMPTY_TILE_BYTE 1

#define NUM_RESERVED 16
#define END_DATA 0

using namespace game;

Map::SharedPtr readMapSimple (const std::string& path, GameObject::SharedPtr gameObject);

Map::SharedPtr game::readMap (const std::string& path, GameObject::SharedPtr gameObject) {
    // TODO: refactor
    // TODO: replace uint32_t with fast version
    logging::log(LEVEL_DEBUG, path.substr(path.size() - 4, std::string::npos));
    if (path.substr(path.size() - 4, std::string::npos) == ".txt") {
        return readMapSimple(path, std::move(gameObject));
    }

    FILE* file = fopen(path.c_str(), "rb");
    if (file == nullptr) {
        logging::log(LEVEL_ERROR, "Error loading map file at path {}", path);
    }
    char magic[MAGIC_LEN];

    /* header */

    // magic number test
    fread(magic, 1, MAGIC_LEN, file);
    if (magic[0] != 'A' || magic[1] != 'c' || magic[2] != 'M' || magic[3] != 'P') {
        logging::log(LEVEL_ERROR, "File at path {} does not have correct magic number for map file!", path);
        return nullptr;
    }

    // version check
    if (fgetc(file) != 1) {
        logging::log(LEVEL_ERROR, "Map file at path {} has unsupported version!", path);
        return nullptr;
    }
    
    // size of header
    unsigned char headerSize = fgetc(file);
    logging::log(LEVEL_DEBUG, "Header size: {}", headerSize);
    // width and height
    uint32_t dimBuf[2];
    fread ((void*) dimBuf, DIMENSION_SIZE, 2, file);
    unsigned int width = dimBuf[0];
    unsigned int height = dimBuf[1];
    logging::log(LEVEL_DEBUG, "Size of map: {} x {}", width, height);
    // size of tile numbers
    unsigned char tileSize = fgetc(file);

    if (tileSize != 1 && tileSize != 2 && tileSize != 4) {
        logging::log(LEVEL_ERROR, "Map file at path {} has incorrect tile num size!", path);
        return nullptr;
    }
    uint32_t tileBuf;
    fread((void*) &tileBuf, TILE_TYPE_NUM_SIZE, 1, file);
    unsigned int numTiles = tileBuf;
    logging::log(LEVEL_DEBUG, "Num tiles: {}", numTiles);
    fseek(file, headerSize, SEEK_SET);

    /* tile type header */

    std::map<uint32_t, Tile*> tileTypeMap;
    char charBuf[STRING_BUFF_SIZE];
    Tile* emptyTile = gameObject->getTile("empty_tile");

    for (auto i = 0; i < numTiles; i++) {
        // tile number
        bool isValid = true;
        fread((void*) &tileBuf, tileSize, 1, file);
        if (feof(file)) {
            logging::log(LEVEL_ERROR, "Unexpected eof in tile type header of map file at path {}", path);
            return nullptr;
        }
        if (tileBuf < NUM_RESERVED) {
            isValid = false;
        }
        // tile string
        auto j = 0;
        int c;
        do {
            if (j >= STRING_BUFF_SIZE) {
                logging::log(LEVEL_WARNING, "Tile {} in map file at path {} has too long name!", tileBuf, path);
                isValid = false;
                break;
            }

            c = fgetc(file);
            charBuf[j] = (char)c;
            j++;
        } while (c != '\0' && c != EOF);

        if (isValid) {
            Tile* tile = gameObject->getTile(std::string(charBuf));
            if (tile == nullptr) {
                //logging::logf(LEVEL_WARNING, "Unknown tile '%s' in map file at path %s", charBuf, path.c_str());
                tile = emptyTile;
            }
            if (tileTypeMap.count(tileBuf) > 0) {
                logging::log(LEVEL_WARNING, "Duplicate tile {} in map file at path {}!", tileBuf, path);
            } else {
                tileTypeMap[tileBuf] = tile;
            }
        }
    }

    /* data */
    logging::log(LEVEL_INFO, "Reading map file at path {}", path);

    Tile** tiles = new Tile*[width * height];
    Tile** tilePtr = tiles;

    for (auto i = 0; i < width * height; i++) {
        tiles[i] = emptyTile;
    }

    fread ((void*)&tileBuf, tileSize, 1, file);
    auto num = 0;
    while (tileBuf != END_DATA) {
        if (feof(file)) {
            logging::log(LEVEL_WARNING, "Ecountered EOF reading map file at {} before data end.", path);
            break;
        }
        if (num >= width * height) {
            logging::log(LEVEL_WARNING, "Max data read before data end encountered reading map file at {}", path);
            break;
        }
        if (tileBuf != EMPTY_TILE_BYTE) {
            if (tileTypeMap.count(tileBuf) > 0) {
                *tilePtr = tileTypeMap[tileBuf];
            }
        }
        tilePtr++;
        num++;
        fread((void*)&tileBuf, tileSize, 1, file);
    }
    logging::log(LEVEL_INFO, "Finished reading map file");
    Map::SharedPtr map = Map::NewSharedPtr(width, height);
    map->setTiles(tiles);
    return map;
}

Map::SharedPtr readMapSimple (const std::string& path, GameObject::SharedPtr gameObject) {
    std::ifstream file(path);

    if (!file) {
        logging::log(LEVEL_ERROR, "Error loading simple map file \"{}\"", path);
        return nullptr;
    }

    std::string headerStr;
    std::getline(file, headerStr);

    auto header = util::stringSplit(headerStr);

    int width = std::stoi(header[0]);
    int height = std::stoi(header[1]);

    int numTiles = std::stoi(header[2]);

    std::unordered_map<int, Tile*> tileIdMap;
    auto emptyTile = gameObject->getTile("empty_tile");

    for (int i = 0; i < numTiles; i++) {
        std::string line;
        std::getline(file, line);
        auto lineSplit = util::stringSplit(line);
        if (lineSplit.size() < 2) {
            logging::log(LEVEL_WARNING, "Simple map tile line \"{}\" is too short!", line);
        }
        auto t = gameObject->getTile(lineSplit[1]);
        if (t == nullptr) {
            t = emptyTile;
        }

        tileIdMap[std::stoi(lineSplit[0])] = t;
    }

    Tile** tiles = new Tile*[width * height]{emptyTile};

    int y = height - 1;

    for (auto& i : util::readLines(file)) {
        if (y < 0) {
            logging::log(LEVEL_WARNING, "Map file \"{}\" height is incorrect!", path);
            break;
        }
        int x = 0;

        for (auto& j : util::stringSplit(i)) {
            if (x >= width) {
                logging::log(LEVEL_WARNING, "Map file \"{}\" width is incorrect!", path);
            }
            auto id = std::stoi(j);

            if (tileIdMap.contains(id)) {
                tiles[y * width + x] = tileIdMap[id];
            }
            x++;

        }
        y--;
    }
    Map::SharedPtr map = Map::NewSharedPtr(width, height);
    map->setTiles(tiles);

    return map;
}