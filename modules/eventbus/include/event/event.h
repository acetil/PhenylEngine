#pragma once

#include <string>
#include "util/meta.h"
#include "forward.h"

namespace event {
    template<typename T, bool _doDebugLog = true>
    class Event {
    public:
        static constexpr bool DoDebugLog = _doDebugLog;

        constexpr static const std::string& getEventName () {
            return T::name;
        }

        friend T;
    };
}
