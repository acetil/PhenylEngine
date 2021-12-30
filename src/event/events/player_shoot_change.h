#include "event/event.h"
#ifndef PLAYER_SHOOT_CHANGE_H
#define PLAYER_SHOOT_CHANGE_H
namespace event {
    class PlayerShootChangeEvent : public Event<PlayerShootChangeEvent> {
    public:
        inline static const std::string name = "player_shoot_change";
        bool doShoot = false;
        PlayerShootChangeEvent () = default;
        explicit PlayerShootChangeEvent (bool _doShoot) : doShoot(_doShoot) {};
    };
}
#endif //PLAYER_SHOOT_CHANGE_H
