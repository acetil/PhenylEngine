#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <memory>

#include "logging/logging.h"
#include "util/meta.h"
#include "util/smart_help.h"
#include "event.h"

#ifndef EVENT_BUS_H
#define EVENT_BUS_H

// TODO: remove duplication

namespace event {
    /*template<typename T, bool _doDebugLog = true, typename ...Args>
    class Event {
    public:
        using CallbackArgsList = meta::type_list_wrapper<Args...>;

        Event () = default;

        [[nodiscard]]
        std::string getEventName () const {
            const T& underlying = static_cast<const T&>(*this);
            return underlying.name;
        };

        void debugLog () {
            logging::log("Raised event: \"{}\"", getEventName());
        }

        static constexpr bool doDebugLog () {
            return _doDebugLog;
        }

        friend T;
    };*/

    class EventHandlerBase {
    protected:
        bool doCallback;

    public:
        EventHandlerBase () : doCallback(false) {};

        explicit EventHandlerBase (bool _doCallback) : doCallback{_doCallback} {};

        template<class T>
        void handle (T& t);

        template<typename T, typename F>
        void handle (T& t, F f);

        template<typename T>
        bool hasExpired ();
    };

    template<typename T, bool doesCallback = false>
    class EventHandler;

    template<typename T>
    class EventHandler<T, false> : public EventHandlerBase {
    private:
        void (* fun) (T&);

    protected:
        explicit EventHandler (const std::string& name) : EventHandlerBase(false) {
            fun = nullptr;
            this->eventName = name;
        }

    public:
        using type = T;
        std::string eventName;

        EventHandler (void (* fun) (T&), const std::string& name) : EventHandlerBase(false) {
            this->fun = fun;
            this->eventName = name;
        };

        virtual void handle (T& t) {
            fun(t);
        };

        virtual bool hasExpired () {
            return false;
        }
    };

    template<typename T>
    class EventHandler<T, true> : public EventHandlerBase {
    private:
        meta::type_list_tuple<typename T::CallbackArgsList> (* fun) (T&);

    protected:
        explicit EventHandler (const std::string& name) : EventHandlerBase(true) {
            fun = nullptr;
            this->eventName = name;
        }

    public:
        using type = T;
        std::string eventName;

        EventHandler (meta::type_list_tuple<typename T::CallbackArgsList> (* fun) (T&), const std::string& name)
                : EventHandlerBase(true) {
            this->fun = fun;
            this->eventName = name;
        };

        virtual meta::type_list_tuple<typename T::CallbackArgsList> handle (T& t) {
            return fun(t);
        };

        virtual bool hasExpired () {
            return false;
        }
    };

    template<typename T, typename A, bool hasCallback = false>
    class EventHandlerMember;

    template<typename T, typename A>
    class EventHandlerMember<T, A, false> : public EventHandler<T, false> {
    private:
        void (A::*fun) (T&);

        std::weak_ptr<A> obj;
    public:
        EventHandlerMember (void (A::*fun) (T&), const std::string& name, std::weak_ptr<A> obj) : EventHandler<T>(
                name) {
            this->fun = fun;
            this->obj = obj;
        }

        virtual bool hasExpired () {
            return obj.expired();
        }

        virtual void handle (T& t) {
            (obj.lock().get()->*fun)(t);
        }
    };

    template<typename T, typename A>
    class EventHandlerMember<T, A, true> : public EventHandler<T, true> {
    private:
        meta::type_list_tuple<typename T::CallbackArgs> (A::*fun) (T&);

        std::weak_ptr<A> obj;
    public:
        EventHandlerMember (meta::type_list_tuple<typename T::CallbackArgsList> (A::*fun) (T&), const std::string& name,
                            std::weak_ptr<A> obj) : EventHandler<T>(name) {
            this->fun = fun;
            this->obj = obj;
        }

        bool hasExpired () override {
            return obj.expired();
        }

        virtual meta::type_list_tuple<typename T::CallbackArgs> handle (T& t) {
            return (obj.get()->*fun)(t);
        }
    };

    template<class T>
    void EventHandlerBase::handle (T& t) {
        if (doCallback) {
            static_cast<EventHandler<T, true>*>(this)->handle(t);
        } else {
            static_cast<EventHandler<T, false>*>(this)->handle(t);
        }
    }

    template<typename T, typename F>
    void EventHandlerBase::handle (T& t, F f) {
        if (doCallback) {
            auto tup = static_cast<EventHandler<T, true>*>(this)->handle(t);
            std::apply(f, tup);
        } else {
            static_cast<EventHandler<T, false>*>(this)->handle(t);
        }
    }

    template<typename T>
    bool EventHandlerBase::hasExpired () {
        if (doCallback) {
            return static_cast<EventHandler<T, true>*>(this)->hasExpired();
        } else {
            return static_cast<EventHandler<T, false>*>(this)->hasExpired();
        }
    }
    template<typename T>
    void canaryDeleter(EventHandlerBase** ptr) {
        delete (T*)*ptr;
        delete ptr;
    }

class EventBus : public util::SmartHelper<EventBus>{
        private:
        std::unordered_map<std::string, std::vector<std::shared_ptr<EventHandlerBase*>>> handlerMap;
        template <typename T>
        std::shared_ptr<EventHandlerBase*> addHandlerInt (EventHandler<T>* handler, void (*deleter)(EventHandlerBase**)) {
            T val;
            std::string type = val.getEventName();
            if (handlerMap.count(type) == 0) {
                handlerMap[type] = std::vector<std::shared_ptr<EventHandlerBase*>>();
            }
            std::shared_ptr<EventHandlerBase*> handlerPtr = std::shared_ptr<EventHandlerBase*>(new EventHandlerBase*, deleter);
            *handlerPtr = handler;
            handlerMap[type].push_back(handlerPtr);

            return handlerPtr;
        }
        template <typename T>
        std::shared_ptr<EventHandlerBase*> addHandler (T* handler) {
            return addHandlerInt(handler, canaryDeleter<T>);
        }
public:
        template <class T, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        std::shared_ptr<EventHandlerBase*> subscribeHandler (void eventHandler(T&)) {
            T val;
            std::string type = val.getEventName();
            auto* handler = new EventHandler<T>(eventHandler, type);
            //addHandler(handler);
            //EventHandler<T> test(eventHandler, type);
            return addHandler(handler);
        };
        template <class T, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        std::shared_ptr<EventHandlerBase*> subscribeHandler (meta::type_list_tuple<typename T::CallbackArgsList> eventHandler(T&)) {
            T val;
            std::string type = val.getEventName();
            auto* handler = new EventHandler<T>(eventHandler, type);
            //addHandler(handler);
            return addHandler(handler);
        };
        template <class T, class A, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        std::shared_ptr<EventHandlerBase*> subscribeHandler (void (A::*eventHandler)(T&), std::shared_ptr<A> obj) {
            T val;
            std::string type = val.getEventName();
            auto handler = new EventHandlerMember<T, A>(eventHandler, type, obj);
            //addHandler(handler);
            return addHandler(handler);
        }
        template <class T, class A, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        std::shared_ptr<EventHandlerBase*> subscribeHandler (meta::type_list_tuple<typename T::CallbackArgsList>  (A::*eventHandler)(T&), std::shared_ptr<A> obj) {
            T val;
            std::string type = val.getEventName();
            auto handler = new EventHandlerMember<T, A>(eventHandler, type, obj);
            //addHandler(handler);
            return addHandler(handler);
        }
        template <class T, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        void unsubscribeHandler (EventHandler<T>* eventHandler) {
            std::string type = eventHandler->eventName;
            if (handlerMap.count(type) != 0) {
                handlerMap[type].erase(std::find(handlerMap[type].begin(), handlerMap[type].end(), eventHandler));
            }
        };
        template <class T, typename = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        void raiseEvent (const T& t) {
            // TODO: other forms of Event
            if constexpr (T::DoDebugLog) {
                logging::log(LEVEL_DEBUG, "Raised event \"{}\"", t.getEventName());
            }
            std::vector<std::shared_ptr<EventHandlerBase*>>& vec = handlerMap[t.getEventName()];
            vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<EventHandlerBase*>& base){return (*base)->hasExpired<T>();}), vec.end());
            //if (t.getEventType()->immediateEval) {
                for (const auto& handler : vec) {
                    (*handler)->handle(t);
                }
            //};
            //delete t;
        }
        template <class T, typename F, typename = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        void raiseEvent (const T& t, F f) {
            // TODO: other forms of Event
            static_assert(meta::is_callable_list<T::CallbackArgs>(f), "Invalid callback function!");
            if constexpr (T::DoDebugLog) {
                logging::log(LEVEL_DEBUG, "Raised event \"{}\"", t.getEventName());
            }
            std::vector<std::shared_ptr<EventHandlerBase*>>& vec = handlerMap[t.getEventName()];
            vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<EventHandlerBase*>& base){return (*base)->hasExpired<T>();}));
            //if (t.getEventType()->immediateEval) {
            for (const auto& handler : vec) {
                (*handler)->handle(t, f);
            }
            //};
            //delete t;
        }
    };
   
}
#endif