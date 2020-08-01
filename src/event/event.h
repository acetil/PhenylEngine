#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <algorithm>

#include "logging/logging.h"
#ifndef EVENT_H
#define EVENT_H
namespace event {
    enum EventHandlerType {
        function, member
    };
     class EventType {
        public:
        EventType (std::string name, bool async, bool immediateEval) {
            this->name = std::move(name);
            this->async = async;
            this->immediateEval = immediateEval;
        }
        std::string name;
        bool async;
        bool immediateEval;
    };
     template<typename T>
    class Event {
    public:
        Event() = default;
        [[nodiscard]]
        std::string getEventName () const {
            const T& underlying = static_cast<const T&>(*this);
            return underlying.name;
        };

        friend T;
    };
    class EventHandlerBase {
        private:
        EventHandlerType type = function;
        public:
        template<class T> void handle (T& t);
    };
    template<typename T>
    class EventHandler : public EventHandlerBase {
        private:
        void (*fun)(T&);
        protected:
        explicit EventHandler (std::string name) {
            fun = nullptr;
            this->eventName = std::move(name);
        }
        public:
        std::string eventName;
        EventHandler (void (*fun)(T&), std::string name) {
            this->fun = fun;
            this->eventName = std::move(name);
        };
        virtual void handle (T& t) {
            fun(t);
        };
    };
    template<typename T, typename A>
    class EventHandlerMember : public EventHandler<T> {
        private:
        void (A::*fun)(T&);
        A* obj;
        public:
        EventHandlerMember(void (A::*fun)(T&), std::string name, A* obj) : EventHandler<T>(name) {
            this->fun = fun;
            this->obj = obj;
        }
        virtual void handle (T& t) {
            (obj->*fun)(t);
        }
    };
    template<class T> void EventHandlerBase::handle (T& t) {
        static_cast<EventHandler<T>*>(this)->handle(t);
    }
    class EventBus {
        private:
        std::unordered_map<std::string, std::vector<EventHandlerBase*>> handlerMap;
        public:
        template <class T, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        EventHandler<T>* subscribeHandler (void eventHandler(T&)) {
            T val;
            std::string type = val.getEventName();
            auto* handler = new EventHandler<T>(eventHandler, type);
            if (handlerMap.count(type) == 0) {
                handlerMap[type] = std::vector<EventHandlerBase*>();
            }
            handlerMap[type].push_back(handler);
            return handler;
        };
        template <class T, class A, class = std::enable_if<std::is_base_of<Event<T>, T>::value>>
        EventHandlerMember<T, A>* subscribeHandler (void (A::*eventHandler)(T&), A* obj) {
            T val;
            std::string type = val.getEventName();
            auto handler = new EventHandlerMember<T, A>(eventHandler, type, obj);
            if (handlerMap.count(type) == 0) {
                handlerMap[type] = std::vector<EventHandlerBase*>();
            }
            handlerMap[type].push_back(handler);
            return handler;
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
            logging::logf(LEVEL_DEBUG, "Raised event \"%s\"", t.getEventName().c_str());
            //if (t.getEventType()->immediateEval) {
                for (EventHandlerBase* handler : handlerMap[t.getEventName()]) {
                    handler->handle(t);
                }
            //};
            //delete t;
        }
    };
   
}
#endif