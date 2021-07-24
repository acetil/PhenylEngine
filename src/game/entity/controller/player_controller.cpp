#include <math.h>

#include "player_controller.h"
#include "logging/logging.h"

#include "util/string_help.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 0.15f

void game::PlayerController::updateMovement (event::PlayerMovementChangeEvent& event) {
    deltaXForce += event.xForceComp;
    deltaYForce += event.yForceComp;
    //logging::logf(LEVEL_DEBUG, "Updating movement: (%f, %f)", deltaXForce, deltaYForce);
}

void game::PlayerController::controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView) {
   if (entityView.entityId != 0) return; // TODO: remove
   entityView.acceleration += glm::vec2(deltaXForce, deltaYForce);
   deltaXForce = 0;
   deltaYForce = 0;
   auto cursorDisp = gameView.getCamera().getWorldPos(cursorScreenPos) - (glm::vec2)entityView.position;
   float rot = atan2(cursorDisp.x, cursorDisp.y)- M_PI_2;
   entityView.rotation = rot;
   /*if (!hasShot && doShoot) {
       glm::vec2 rotVec = {cos(-rot), sin(-rot)};
       glm::vec2 relPos = rotVec * SHOOT_DIST;
       auto bulletId = gameView.createEntityInstance("bullet", entityView.position().x + relPos.x,
                                                     entityView.position().y + relPos.y);
       auto bulletView = entityView.withId(bulletId);
       bulletView.rotation = rot;
       bulletView.velocity = rotVec * SHOOT_VEL;
       hasShot = true;
   }*/
}

void game::PlayerController::updateCursorPos (event::CursorPosChangeEvent &event) {
    cursorScreenPos = event.screenPos;
}

void game::PlayerController::updateDoShoot (event::PlayerShootChangeEvent &event) {
    doShoot = event.doShoot;
    hasShot &= doShoot;
}

int game::PlayerController::getTextureId (view::EntityView& entityView, view::GameView& gameView) {
    return texId;
}

void game::PlayerController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test8");
}

void game::PlayerController::controlEntityPostPhysics (view::EntityView& entityView, view::GameView& gameView) {
    if (!hasShot && doShoot) {
        auto rot = entityView.rotation;
        glm::vec2 rotVec = {cos(-rot()), sin(-rot())};
        glm::vec2 relPos = rotVec * SHOOT_DIST;
        glm::vec2 bulletVel = rotVec * SHOOT_VEL;

        util::DataObject bData;
        util::DataObject bVel;
        bVel["x"] = bulletVel.x;
        bVel["y"] = bulletVel.y;

        bData["vel"] = bVel;

        gameView.createEntityInstance("bullet", entityView.position().x + relPos.x,
                                                      entityView.position().y + relPos.y, rot(), util::DataValue(bData));
        /*auto bulletView = entityView.withId(bulletId);
        //bulletView.rotation = rot(); // TODO: look into difference with ()
        bulletView.velocity = rotVec * SHOOT_VEL;*/
        hasShot = true;
    }
    gameView.getCamera().setPos(entityView.position());
}


