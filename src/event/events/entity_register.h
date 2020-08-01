#include "event/event.h"
#include "game/game_object.h"
#ifndef ENTITY_REGISTER_H
#define ENTITY_REGISTER_H
namespace event {
    class EntityRegisterEvent : public Event<EntityRegisterEvent> {
    public:
        std::string name = "entity_register";
        game::GameObject* gameObject{};
        EntityRegisterEvent () = default;
        explicit EntityRegisterEvent (game::GameObject* gameObject) {
            this->gameObject = gameObject;
        }
    };
}
#endif