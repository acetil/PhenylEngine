#include "event/event.h"
#ifndef PLAYER_MOVEMENT_CHANGE_H
#define PLAYER_MOVEMENT_CHANGE_H
namespace event {
    class PlayerMovementChangeEvent : public Event<PlayerMovementChangeEvent, false> {
    public:
        inline static const std::string name = "player_movement_change";
        float xForceComp;
        float yForceComp;
        PlayerMovementChangeEvent () {
            xForceComp = 0;
            yForceComp = 0;
        }
        PlayerMovementChangeEvent (float xForce, float yForce) {
            this->xForceComp = xForce;
            this->yForceComp = yForce;
        }
    };
}
#endif //PLAYER_MOVEMENT_CHANGE_H
