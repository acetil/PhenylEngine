#include "event/event.h"
#ifndef RELOAD_MAP_H
#define RELOAD_MAP_H
namespace event {
    class ReloadMapEvent : public Event<ReloadMapEvent, true> {
    public:
        std::string name = "map_reload";
        ReloadMapEvent() = default;
    };
}
#endif
