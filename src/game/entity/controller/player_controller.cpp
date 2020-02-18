#include "player_controller.h"
#include "logging/logging.h"
void game::PlayerController::updateMovement (event::PlayerMovementChangeEvent* event) {
    deltaXForce += event->xForceComp;
    deltaYForce += event->yForceComp;
    logging::logf(LEVEL_DEBUG, "Updating movement: (%f, %f)", deltaXForce, deltaYForce);
}

void game::PlayerController::controlEntityPrePhysics (game::AbstractEntity* entity, component::EntityMainComponent* comp,
                                                      int entityId, component::ComponentManager* manager) {
    //logging::log(LEVEL_DEBUG, "Updating acceleration!");
    comp->acc[0] += deltaXForce;
    comp->acc[1] += deltaYForce;
    deltaXForce = 0;
    deltaYForce = 0;
}


