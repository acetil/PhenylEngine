#include <math.h>

#include "player_controller.h"
#include "logging/logging.h"

void game::PlayerController::updateMovement (event::PlayerMovementChangeEvent& event) {
    deltaXForce += event.xForceComp;
    deltaYForce += event.yForceComp;
    //logging::logf(LEVEL_DEBUG, "Updating movement: (%f, %f)", deltaXForce, deltaYForce);
}

void game::PlayerController::controlEntityPrePhysics (view::EntityView& entityView) {
   if (entityView.entityId != 0) return; // TODO: remove
   entityView.acceleration += glm::vec2(deltaXForce, deltaYForce);
   deltaXForce = 0;
   deltaYForce = 0;
   entityView.rotation += M_PI / 60;
}


