#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include "event/events/player_movement_change.h"
#include "entity_controller.h"

namespace game {
    class PlayerController : public EntityController {
    private:
        float deltaXForce;
        float deltaYForce;
    public:
        PlayerController () {
            deltaXForce = 0;
            deltaYForce = 0;
        }
        void updateMovement (event::PlayerMovementChangeEvent* event);
        void controlEntityPrePhysics (AbstractEntity* entity, component::EntityMainComponent *comp, int entityId,
                                              component::ComponentManager<AbstractEntity*>* manager) override;

    };
}
#endif
