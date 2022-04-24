#pragma once

#include "event/event.h"

namespace event {
    class PlayerMovementChangeEvent : public Event<PlayerMovementChangeEvent, false> {
    public:
        inline static const std::string name = "player_movement_change";
        float xForceComp;
        float yForceComp;
        /*PlayerMovementChangeEvent () {
            xForceComp = 0;
            yForceComp = 0;
        }*/
        PlayerMovementChangeEvent (float xForce, float yForce) {
            this->xForceComp = xForce;
            this->yForceComp = yForce;
        }
    };
}
