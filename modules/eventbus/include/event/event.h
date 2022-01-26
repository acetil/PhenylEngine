#pragma once

#include <string>
#include "util/meta.h"

namespace event {
    template<typename T, bool _doDebugLog = true, typename ...Args>
    class Event {
    public:
        using CallbackArgsList = meta::type_list_wrapper<Args...>;

        static constexpr bool DoDebugLog = _doDebugLog;

        //Event () = default;

        constexpr static const std::string& getEventName () {
            return T::name;
        }

        /*void debugLog () {
            logging::log("Raised event: \"{}\"", getEventName());
        }*/

        /*static constexpr bool doDebugLog () {
            return _doDebugLog;
        }*/

        friend T;
    };
}
