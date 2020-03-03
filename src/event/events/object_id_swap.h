#include <unordered_map>
#include <string>
#include "event/event.h"
#ifndef ENTITY_ID_SWAP_H
#define ENTITY_ID_SWAP_H
namespace event {
    static EventType* ENTITY_ID_SWAP_EVENT = nullptr;
    static std::unordered_map<std::string, EventType*> OBJECT_SWAP_EVENT_TYPES;
    template <class T>
    class ObjectIdSwapEvent : public Event {
        public:
        int oldId;
        int newId;
        std::string objectType;
        ObjectIdSwapEvent () {
            objectType = T::getObjectType();
            oldId = 0;
            newId = 0;
        }
        ObjectIdSwapEvent (int oldId, int newId) {
            objectType = T::getObjectType();
            this->oldId = oldId;
            this->newId = newId;
        };
        virtual EventType* getEventType () {
            if (OBJECT_SWAP_EVENT_TYPES.count(objectType) == 0) {
                OBJECT_SWAP_EVENT_TYPES[objectType] = new EventType("'" + objectType + "' id_swap", false, true);
            }
            return OBJECT_SWAP_EVENT_TYPES[objectType];
        }
    };
}
#endif