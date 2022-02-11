#pragma once

#include <utility>

#include "event/event.h"
//#include "engine/game_object.h"
#include "engine/phenyl_game.h"

namespace event {
    class EntityRegisterEvent : public Event<EntityRegisterEvent> {
    public:
        inline static const std::string& name = "entity_register";
        game::PhenylGame gameObject;
        //EntityRegisterEvent () = default;
        explicit EntityRegisterEvent (game::PhenylGame _gameObject) : gameObject{std::move(_gameObject)} {}
    };
}
