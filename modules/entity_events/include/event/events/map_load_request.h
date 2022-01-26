#pragma once

#include "event/event.h"

namespace event {
    class MapLoadRequestEvent : public Event<MapLoadRequestEvent> {
    public:
        inline static const std::string name = "map_load_request";
        const std::string& filepath;
        //MapLoadRequestEvent() = default;
        explicit MapLoadRequestEvent(std::string& _filepath) : filepath(_filepath) {}
    };
}
