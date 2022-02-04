#pragma once

#include <utility>

#include "event/event.h"
#include "engine/game_object.h"
#include "game_object.h"

namespace event {
    class EntityRegisterEvent : public Event<EntityRegisterEvent> {
    public:
        inline static const std::string name = "entity_register";
        game::detail::GameObject::SharedPtr gameObject{};
        //EntityRegisterEvent () = default;
        explicit EntityRegisterEvent (game::detail::GameObject::SharedPtr gameObject) {
            this->gameObject = std::move(gameObject);
        }
    };
}
