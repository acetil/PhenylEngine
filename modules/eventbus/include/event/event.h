#include <string>
#include "util/meta.h"

#ifndef EVENT_H
#define EVENT_H
namespace event {
    template<typename T, bool _doDebugLog = true, typename ...Args>
    class Event {
    public:
        using CallbackArgsList = meta::type_list_wrapper<Args...>;

        static constexpr bool DoDebugLog = _doDebugLog;

        Event () = default;

        [[nodiscard]]
        constexpr const std::string& getEventName () const {
            //const T& underlying = static_cast<const T&>(*this);
            //return underlying.name;
            return T::name;
        };

        /*void debugLog () {
            logging::log("Raised event: \"{}\"", getEventName());
        }*/

        /*static constexpr bool doDebugLog () {
            return _doDebugLog;
        }*/

        friend T;
    };
}
#endif //EVENT_H
