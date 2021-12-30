#include "event/event.h"
#include "game/map/map.h"

#ifndef MAP_LOAD_H
#define MAP_LOAD_H
namespace event {
    class MapLoadEvent : public Event<MapLoadEvent, true> {
    public:
        inline static const std::string name = "map_load";
        game::Map::SharedPtr map = nullptr;
        MapLoadEvent () = default;
        explicit MapLoadEvent (game::Map::SharedPtr _map) : map(std::move(_map)) {};
    };
}
#endif
