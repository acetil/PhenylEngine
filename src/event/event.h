#include <string>
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
            this->name = name;
            this->async = async;
            this->immediateEval = immediateEval;
        }
        std::string name;
        bool async;
        bool immediateEval;
    };
    class Event {
        public:
        virtual EventType* getEventType () = 0;
    };
    class EventHandlerBase {
        private:
        EventHandlerType type;
        public:
        template<class T> void handle (T* t);
        virtual EventType* getEventType () = 0;
    };
    template<typename T>
    class EventHandler : public EventHandlerBase {
        private:
        void (*fun)(T*);
        protected:
        EventType* type;
        EventHandler (EventType* type) {
            fun = nullptr;
            this->type = type;
        }
        public:
        EventHandler (void (*fun)(T*), EventType* type) {
            this->fun = fun;
            this->type = type;
        };
        virtual void handle (T* t) {
            fun(t);
        };
        virtual EventType* getEventType () {
            return type;
        }
    };
    template<typename T, typename A>
    class EventHandlerMember : public EventHandler<T> {
        private:
        void (A::*fun)(T*);
        A* obj;
        public:
        EventHandlerMember(void (A::*fun)(T*), EventType* type, A* obj) : EventHandler<T>{type}
         {
            this->fun = fun;
            this->obj = obj;
        }
        virtual void handle (T* t) {
            (obj->*fun)(t);
        }
    };
    template<class T> void EventHandlerBase::handle (T* t) {
        dynamic_cast<EventHandler<T>*>(this)->handle(t);
    }
    class EventBus {
        private:
        std::unordered_map<EventType*, std::vector<EventHandlerBase*>> handlerMap;
        public:
        template <class T, class = std::enable_if<std::is_base_of<Event, T>::value>>
        EventHandler<T>* subscribeHandler (void eventHandler(T*)) {
            T val;
            EventType* type = val.getEventType();
            EventHandler<T>* handler = new EventHandler<T>(eventHandler, type);
            if (handlerMap.count(type) == 0) {
                handlerMap[type] = std::vector<EventHandlerBase*>();
            }
            handlerMap[type].push_back(handler);
            return handler;
        };
        template <class T, class A, class = std::enable_if<std::is_base_of<Event, T>::value>>
        EventHandlerMember<T, A>* subscribeHandler (void (A::*eventHandler)(T*), A* obj) {
            T val;
            EventType* type = val.getEventType();
            EventHandlerMember<T, A>* handler = new EventHandlerMember<T, A>(eventHandler, type, obj);
            if (handlerMap.count(type) == 0) {
                handlerMap[type] = std::vector<EventHandlerBase*>();
            }
            handlerMap[type].push_back(handler);
            return handler;
        }
        template <class T, class = std::enable_if<std::is_base_of<Event, T>::value>>
        void unsubscribeHandler (EventHandler<T>* eventHandler) {
            EventType* type = eventHandler.getEventType();
            if (handlerMap.count(type) != 0) {
                handlerMap[type].erase(std::find(handlerMap[type].begin(), handlerMap[type].end(), eventHandler));
            }
        };
        template <class T, typename = std::enable_if<std::is_base_of<Event, T>::value>>
        void raiseEvent (T* t) {
            // TODO: other forms of Event
            logging::logf(LEVEL_DEBUG, "Raised event \"%s\"", t->getEventType()->name.c_str());
            if (t->getEventType()->immediateEval) {
                for (EventHandlerBase* handler : handlerMap[t->getEventType()]) {
                    handler->handle(t);
                }
            };
            delete t;
        }
    };
   
}
#endif