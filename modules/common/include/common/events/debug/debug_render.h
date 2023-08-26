#pragma once

#include "event/event.h"

namespace event {
    struct DebugRenderEvent : public Event<DebugRenderEvent> {
        inline static const std::string name = "debug_render";
        bool doRender;

        DebugRenderEvent (bool _doRender) : doRender{_doRender} {}
    };
}