#include <optional>

#include "event/event.h"

#ifndef TOGGLE_PROFILER_H
#define TOGGLE_PROFILER_H
namespace event {
    class ProfilerChangeEvent : public Event <ProfilerChangeEvent>{
    public:
        inline static const std::string name = "profiler_change";
        std::optional<bool> doDisplay;
        //ProfilerChangeEvent () : doDisplay(std::nullopt) {}
        explicit ProfilerChangeEvent (std::optional<bool> _doDisplay) : doDisplay(_doDisplay) {}
    };
}
#endif
