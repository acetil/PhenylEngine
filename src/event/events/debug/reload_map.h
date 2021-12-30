#include "event/event.h"
#ifndef RELOAD_MAP_H
#define RELOAD_MAP_H
namespace event {
    class ReloadMapEvent : public Event<ReloadMapEvent, true> {
    public:
        inline static const std::string name = "map_reload";
        ReloadMapEvent() = default;
    };
}
#endif
