#include "event/event.h"
#ifndef DUMP_MAP_H
#define DUMP_MAP_H
namespace event {
    class DumpMapEvent : public Event<DumpMapEvent> {
    public:
        inline static const std::string name = "dump_map";
        const std::string& filepath;
        //DumpMapEvent () = default;
        explicit DumpMapEvent (const std::string& path) : filepath(path) {}
    };
}
#endif
