#include "event/event.h"
#include "game/game_object.h"
#ifndef ENTITY_REGISTER_H
#define ENTITY_REGISTER_H
namespace event {
static EventType* ENTITY_REGISTER_EVENT = nullptr;
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
}
#endif