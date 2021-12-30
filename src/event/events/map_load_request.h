#include "event/event.h"

#ifndef MAP_LOAD_REQUEST_H
#define MAP_LOAD_REQUEST_H
namespace event {
    class MapLoadRequestEvent : public Event<MapLoadRequestEvent> {
    public:
        inline static const std::string name = "map_load_request";
        const std::string& filepath;
        //MapLoadRequestEvent() = default;
        explicit MapLoadRequestEvent(std::string& _filepath) : filepath(_filepath) {}
    };
}
#endif
