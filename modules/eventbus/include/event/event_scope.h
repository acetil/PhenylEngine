#pragma once

#include "common/scope.h"
#include "forward.h"

namespace event {
    namespace detail {
        class EventScopeObserver;
    }

    class EventScope {
    private:
        common::Scope scope;

        void initScope () {
            scope.initScope();
        }

    public:
        EventScope () = default;

        operator bool () {
            return (bool) scope;
        }

        friend class detail::EventScopeObserver;
        friend class detail::EventBus;
    };

    namespace detail {
        class EventScopeObserver {
        private:
            common::ScopeObserver scopeObserver{};
            bool hasScope;
        public:
            EventScopeObserver () : hasScope{false} {};

            EventScopeObserver (const EventScope& scope) : scopeObserver{scope.scope} {
                hasScope = scopeObserver;
            }

            EventScopeObserver& operator= (const EventScope& scope) {
                scopeObserver = scope.scope;
                hasScope = scopeObserver;

                return *this;
            }

            operator bool () const {
                return !hasScope || (bool) scopeObserver;
            }
        };
    }
}