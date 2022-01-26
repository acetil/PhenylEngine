#pragma once

#include "event/event.h"

namespace event {
    class ReloadMapEvent : public Event<ReloadMapEvent, true> {
    public:
        inline static const std::string name = "map_reload";
        //ReloadMapEvent() = default;
    };
}
