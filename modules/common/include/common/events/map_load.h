#pragma once

#include "event/event.h"
#include "engine/map/map.h"

namespace event {
    class MapLoadEvent : public Event<MapLoadEvent, true> {
    public:
        inline static const std::string name = "map_load";
        game::Map::SharedPtr map = nullptr;
        //MapLoadEvent () = default;
        explicit MapLoadEvent (game::Map::SharedPtr _map) : map(std::move(_map)) {};
    };
}

