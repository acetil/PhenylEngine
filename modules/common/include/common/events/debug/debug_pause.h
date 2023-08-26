#pragma once

#include "event/event.h"

namespace event {
    class DebugPauseEvent : public Event<DebugPauseEvent> {
    public:
        inline static const std::string name = "debug_pause";
        bool doPause;

        explicit DebugPauseEvent (bool doPause) : doPause{doPause} {}
    };
}