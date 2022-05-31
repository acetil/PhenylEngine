#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <memory>

#include "forward.h"
#include "logging/logging.h"
#include "util/meta.h"
#include "util/map.h"
#include "util/smart_help.h"
#include "event.h"
#include "event_scope.h"
#include "common/scope.h"

namespace event {
    namespace detail {

        template <typename T>
        static constexpr bool is_event_impl = std::disjunction_v<std::is_base_of<Event<T, false>, T>, std::is_base_of<Event<T, true>, T>>;

        template <typename T>
        static constexpr bool is_event = is_event_impl<std::remove_cvref_t<T>>;

        class EventHandlerBase {
        public:
            template <typename T>
            void _handle (T& t);

            template <typename T>
            [[nodiscard]] bool _hasExpired () const;

            virtual ~EventHandlerBase() = default;
        };

        template <typename T>
        class EventHandler : public event::detail::EventHandlerBase {
        private:
            EventScopeObserver scopeObserver;

        public:
            explicit EventHandler (EventScopeObserver observer) : scopeObserver{std::move(observer)} {}

            virtual void handle (T& t) = 0;

            [[nodiscard]] virtual bool hasExpired () const {
                return !scopeObserver;
            }
        };

        template <typename T, class O>
        class EventMemberHandler : public event::detail::EventHandler<T> {
        public:
            typedef void (O::*FuncType) (T&);
        private:
            FuncType func;
            O* obj;
        public:
            EventMemberHandler (FuncType _func, O* _obj, EventScopeObserver observer) : event::detail::EventHandler<T>(observer), func{_func}, obj{_obj} {}

            void handle (T& t) override {
                (obj->*func)(t);
            }
        };

        template <typename T>
        class EventFunctionHandler : public event::detail::EventHandler<T> {
        private:
            std::function<void(T&)> func;
        public:
            EventFunctionHandler (const std::function<void(T&)>& _func, EventScopeObserver observer) : event::detail::EventHandler<T>(observer), func{_func} {}

            void handle (T& t) override {
                func(t);
            }
        };

        template <typename T>
        void event::detail::EventHandlerBase::_handle (T& t) {
            static_cast<EventHandler<T>*>(this)->handle(t);
        }

        template <typename T>
        bool event::detail::EventHandlerBase::_hasExpired () const {
            return static_cast<const EventHandler<T>*>(this)->hasExpired();
        }

        class EventBus : public util::SmartHelper<EventBus> {
        private:
            util::Map<std::size_t, std::vector<std::unique_ptr<EventHandlerBase>>> handlerMap;


            template <typename T>
            void addHandler (std::unique_ptr<detail::EventHandlerBase> handler) {
                std::size_t index = meta::type_index<std::remove_cvref_t<T>>();

                if (!handlerMap.contains(index)) {
                    handlerMap[index] = std::vector<std::unique_ptr<EventHandlerBase>>{};
                }
                handlerMap[meta::type_index<std::remove_cvref_t<T>>()].emplace_back(std::move(handler));
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void raiseEventInt (T& event) {
                std::size_t eIndex = meta::type_index<std::remove_cvref_t<T>>();

                if constexpr (T::DoDebugLog) {
                    logging::log(LEVEL_DEBUG, "Raised event \"{}\"", T::getEventName());
                }

                if (!handlerMap.contains(eIndex)) {
                    return;
                }

                std::vector<std::unique_ptr<event::detail::EventHandlerBase>>& handlerVec = handlerMap.at(eIndex);

                handlerVec.erase(std::remove_if(handlerVec.begin(), handlerVec.end(), [] (const std::unique_ptr<event::detail::EventHandlerBase>& i) {
                    return i->_hasExpired<T>();
                }), handlerVec.end());

                for (auto& i : handlerVec) {
                    if (!i->_hasExpired<T>()) {
                        i->_handle<T>(event);
                    }
                }
            }

        public:
            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            [[nodiscard]] EventScope subscribe (std::function<void(T&)> func) {
                EventScope scope = getScope();

                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, scope));

                return scope;
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void subscribe (std::function<void(T&)> func, const EventScope& scope) {
                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, scope));
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void subscribeUnscoped (std::function<void(T&)> func) {
                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, EventScopeObserver()));
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            [[nodiscard]] EventScope subscribe (void (*func) (T&)) {
                EventScope scope = getScope();

                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, scope));

                return scope;
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void subscribe (void (*func) (T&), const EventScope& scope) {
                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, scope));
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void subscribeUnscoped (void (*func) (T&)) {
                addHandler<T>(std::make_unique<EventFunctionHandler<T>>(func, EventScopeObserver()));
            }

            template <typename T, class O, std::enable_if_t<is_event<T>, bool> = true>
            [[nodiscard]] EventScope subscribe (void (O::*func) (T&), O* obj) {
                EventScope scope = getScope();

                addHandler<T>(std::make_unique<EventMemberHandler<T, O>>(func, obj, scope));

                return scope;
            }

            template <typename T, class O, std::enable_if_t<is_event<T>, bool> = true>
            void subscribe (void (O::*func) (T&), O* obj, const EventScope& scope) {
                addHandler<T>(std::make_unique<EventMemberHandler<T, O>>(func, obj, scope));
            }
            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void raise (T& event) {
                raiseEventInt(event);
            }

            template <typename T, std::enable_if_t<is_event<T>, bool> = true>
            void raise (T&& event) {
                T eventInstance = event;
                raiseEventInt(eventInstance);
            }

            [[nodiscard]] EventScope getScope () const {
                EventScope scope;
                scope.initScope();

                return scope;
            }
        };
    }
   
}
