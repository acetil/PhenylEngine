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
   auto cursorDisp = cursorWorldPos - entityView.position();
   entityView.rotation = atan2(cursorDisp.x, cursorDisp.y) - M_PI_2;
}

void game::PlayerController::updateCursorPos (event::CursorPosChangeEvent &event) {
    cursorWorldPos = event.worldPos;
}


