#include "event.h"
#ifndef EVENTS_H
#define EVENTS_H
namespace event {
    static EventType* ENTITY_REGISTER_EVENT = nullptr;
    class EntityRegisterEvent : Event {
        public:
        virtual EventType* getEventType () {
            if (ENTITY_REGISTER_EVENT == nullptr) {
                ENTITY_REGISTER_EVENT = new EventType("entity_register", false, true);
            }
            return ENTITY_REGISTER_EVENT;
        }
    };
}
#endif