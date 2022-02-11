/*
    DEFINITION OF MAP FILE (.acmp) VERSION 1
    bytes : meaning
    all numbers are little endian
    0-3: AcMP (in that capitalisation)
    4: version (currently only 1 is supported)
    5: size of header (h)
    6-9: width of map (unsigned int) (w)
    10-13: height of map (unsigned int) (l)
    14: size of tile numbers (1: byte 2: short 4: int) (a)
    15-18: number of tile types (unsigned int) (n)

    after l n times this:
    a bytes defining the number, then a null-terminated string being the name of the tile (max 256 chars inc null)

    0-15 are reserved
    special numbers:
    0: defines the end of the tile map
    1: empty tile
    after that, UP TO w * h tiles. The end will be marked by 0. All other bytes past that will be discarded.
    Unknown tiles are to be replaced (silently) with empty tile. If EOF is encountered before 0, warn and treat eof as 0

*/
#pragma once

#include <string>

#include "engine/map/map.h"
//#include "engine/game_object.h"
#include "engine/phenyl_game.h"

namespace game {
    Map::SharedPtr readMap (const std::string& path, game::PhenylGame gameObject);
    Map::SharedPtr readMapNew (const std::string& path, game::PhenylGame gameObject);
}
