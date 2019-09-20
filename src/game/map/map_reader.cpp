#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include "map_reader.h"
#include "map.h"

#include "game/tile/tile.h"
#include "logging/logging.h"

#define MAGIC_LEN 4
#define DIMENSION_SIZE 4
#define TILE_TYPE_NUM_SIZE 4

#define STRING_BUFF_SIZE 256

#define EMPTY_TILE_NUM 0
#define EMPTY_TILE_BYTE 1

#define NUM_RESERVED 16
#define END_DATA 0

using namespace game;

typedef struct _tile {
    std::string name;
    uint32_t num;
    Tile* tile;
} tileType;
Map* game::readMap (std::string path, GameObject* gameObject) {
    // TODO: refactor
    // TODO: replace uint32_t with fast version
    FILE* file = fopen(path.c_str(), "rb");
    if (file == NULL) {
        logging::logf(LEVEL_ERROR, "Error loading map file at path %s", path.c_str());
    }
    char magic[MAGIC_LEN];

    /* header */

    // magic number test
    fread(magic, 1, MAGIC_LEN, file);
    if (magic[0] != 'A' || magic[1] != 'c' || magic[2] != 'M' || magic[3] != 'P') {
        logging::logf(LEVEL_ERROR, "File at path %s does not have correct magic number for map file!", path.c_str());
        return NULL;
    }

    // version check
    if (fgetc(file) != 1) {
        logging::logf(LEVEL_ERROR, "Map file at path %s has unsupported version!", path.c_str());
        return NULL;
    }
    
    // size of header
    unsigned char headerSize = fgetc(file);

    // width and height
    uint32_t dimBuf[2];
    fread ((void*) dimBuf, DIMENSION_SIZE, 2, file);
    unsigned int width = dimBuf[0];
    unsigned int height = dimBuf[1];

    // size of tile numbers
    unsigned char tileSize = fgetc(file);

    if (tileSize != 1 && tileSize != 2 && tileSize != 4) {
        logging::logf(LEVEL_ERROR, "Map file at path %s has incorrect tile num size!", path.c_str());
        return NULL;
    }
    uint32_t tileBuf;
    fread((void*) &tileBuf, TILE_TYPE_NUM_SIZE, 1, file);
    unsigned int numTiles = tileBuf;
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
            logging::logf(LEVEL_ERROR, "Unexpected eof in tile type header of map file at path %s", path.c_str());
            return NULL;
        }
        if (tileBuf < NUM_RESERVED) {
            isValid = false;
        }
        // tile string
        auto j = 0;
        char c;
        do {
            if (j >= STRING_BUFF_SIZE) {
                logging::logf(LEVEL_WARNING, "Tile %u in map file at path %s has too long name!", tileBuf, path.c_str());
                isValid = false;
                break;
            }

            c = fgetc(file);
            charBuf[j] = c;
            j++;
        } while (c != '\0');

        if (isValid) {
            Tile* tile = gameObject->getTile(std::string(charBuf));
            if (tile == NULL) {
                //logging::logf(LEVEL_WARNING, "Unknown tile '%s' in map file at path %s", charBuf, path.c_str());
                tile = emptyTile;
            }
            if (tileTypeMap.count(tileBuf) > 0) {
                logging::logf(LEVEL_WARNING, "Duplicate tile %u in map file at path %s!", tileBuf);
            } else {
                tileTypeMap[tileBuf] = tile;
            }
        }
    }

    /* data */
    logging::logf(LEVEL_INFO, "Reading map file at path %s", path.c_str());

    Tile** tiles = new Tile*[width * height];
    Tile** tilePtr = tiles;

    for (auto i = 0; i < width * height; i++) {
        tiles[i] = emptyTile;
    }

    fread ((void*)&tileBuf, tileSize, 1, file);
    auto num = 0;
    while (tileBuf != END_DATA) {
        if (feof(file)) {
            logging::logf(LEVEL_WARNING, "Ecountered EOF reading map file at %s before data end.", path.c_str());
            break;
        }
        if (num >= width * height) {
            logging::logf(LEVEL_WARNING, "Max data read before data end encountered reading map file at %s", path.c_str());
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
    Map* map = new Map(width, height);
    map->setTiles(tiles);
    return map;
}   