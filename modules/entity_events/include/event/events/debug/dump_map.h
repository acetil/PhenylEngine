#pragma once

#include "event/event.h"

namespace event {
    class DumpMapEvent : public Event<DumpMapEvent> {
    public:
        inline static const std::string name = "dump_map";
        const std::string& filepath;
        //DumpMapEvent () = default;
        explicit DumpMapEvent (const std::string& path) : filepath(path) {}
    };
}
