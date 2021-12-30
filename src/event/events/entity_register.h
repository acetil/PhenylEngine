#include <utility>

#include "event/event.h"
#include "game/game_object.h"
#ifndef ENTITY_REGISTER_H
#define ENTITY_REGISTER_H
namespace event {
    class EntityRegisterEvent : public Event<EntityRegisterEvent> {
    public:
        inline static const std::string name = "entity_register";
        game::GameObject::SharedPtr gameObject{};
        EntityRegisterEvent () = default;
        explicit EntityRegisterEvent (game::GameObject::SharedPtr gameObject) {
            this->gameObject = std::move(gameObject);
        }
    };
}
#endif