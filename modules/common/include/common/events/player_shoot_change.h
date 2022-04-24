#pragma once

#include "event/event.h"

namespace event {
    class PlayerShootChangeEvent : public Event<PlayerShootChangeEvent> {
    public:
        inline static const std::string name = "player_shoot_change";
        bool doShoot = false;
        //PlayerShootChangeEvent () = default;
        explicit PlayerShootChangeEvent (bool _doShoot) : doShoot(_doShoot) {};
    };
}
