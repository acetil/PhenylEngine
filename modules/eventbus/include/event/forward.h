#pragma once

namespace event {
    template <typename T, bool _doDebugLog>
    class Event;

    class EventScope;

    namespace detail {
        class EventBus;
    }

    using EventBus = detail::EventBus;
}