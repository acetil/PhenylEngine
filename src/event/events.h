#include "event.h"
#include "game/entity/entity.h"
#include "game/game_object.h"
#include "logging/logging.h"
#ifndef EVENTS_H
#define EVENTS_H
namespace event {
    static EventType* ENTITY_REGISTER_EVENT = nullptr;
    static EventType* ENTITY_ID_SWAP_EVENT = nullptr;
    class EntityRegisterEvent : public Event {
        public:
        game::GameObject* gameObject;
        EntityRegisterEvent () {

       }
       EntityRegisterEvent (game::GameObject* gameObject) {
           this->gameObject = gameObject;
       }
        virtual EventType* getEventType () {
            if (ENTITY_REGISTER_EVENT == nullptr) {
                ENTITY_REGISTER_EVENT = new EventType("entity_register", false, true);
            }
            return ENTITY_REGISTER_EVENT;
        }
    };
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