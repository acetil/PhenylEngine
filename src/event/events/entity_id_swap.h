#include "event/event.h"
#ifndef ENTITY_ID_SWAP_H
#define ENTITY_ID_SWAP_H
namespace event {
    static EventType* ENTITY_ID_SWAP_EVENT = nullptr;
    class EntityIdSwapEvent : public Event {
        public:
        int oldId;
        int newId;
        EntityIdSwapEvent () {
            oldId = 0;
            newId = 0;
        }
        EntityIdSwapEvent (int oldId, int newId) {
            this->oldId = oldId;
            this->newId = newId;
        };
        virtual EventType* getEventType () {
            if (ENTITY_ID_SWAP_EVENT == nullptr) {
                ENTITY_ID_SWAP_EVENT = new EventType("entity_id_swap", false, true);
            }
            return ENTITY_ID_SWAP_EVENT;
        }
    };
}
#endif