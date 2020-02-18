#include "event/event.h"
#ifndef PLAYER_MOVEMENT_CHANGE_H
#define PLAYER_MOVEMENT_CHANGE_H
namespace event {
    static EventType* PLAYER_MOVEMENT_CHANGE_EVENT;
    class PlayerMovementChangeEvent : public Event {
        public:
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
        EventType* getEventType () override {
            if (PLAYER_MOVEMENT_CHANGE_EVENT == nullptr) {
                PLAYER_MOVEMENT_CHANGE_EVENT = new EventType("player_movement_change", false, true);
            }
            return PLAYER_MOVEMENT_CHANGE_EVENT;
        }
    };
}
#endif //PLAYER_MOVEMENT_CHANGE_H
