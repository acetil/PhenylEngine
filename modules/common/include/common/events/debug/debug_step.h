#pragma once

#include "event/event.h"

namespace event {
    enum class DebugStepStatus {
        ENABLE_STEPPING,
        DISABLE_STEPPING,
        DO_STEP
    };
    struct DebugStepEvent : Event<DebugStepEvent, false> {
        inline static const std::string name = "debug_step";
        DebugStepStatus status;
    };
}