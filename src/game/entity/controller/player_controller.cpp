#include <math.h>

#include "engine/entity/controller/player_controller.h"
#include "logging/logging.h"

#include "util/string_help.h"
#include "engine/game_camera.h"
#include "graphics/textures/texture_atlas.h"
#include "physics/components/2D/rigid_body.h"
#include "common/components/2d/global_transform.h"

#include "game/entity/player.h"
#include "component/component_serialiser.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 7.5f

void game::PlayerController::updateMovement (event::PlayerMovementChangeEvent& event) {
    currForce += glm::vec2{event.xForceComp, event.yForceComp};
    //logging::logf(LEVEL_DEBUG, "Updating movement: (%f, %f)", deltaXForce, deltaYForce);
}

void game::PlayerController::controlEntityPrePhysics (component::EntityView& entityView, view::GameView& gameView) {
   //if (entityView.entityId != 0) return; // TODO: remove

   glm::vec2 cursorPos = gameView.getCamera().getWorldPos(cursorScreenPos);

    entityView.get<physics::RigidBody2D>().ifPresent([this](physics::RigidBody2D& comp) {
       comp.applyForce(comp.getMass() * currForce);
   });

    /*entityView.get<component::Position2D>().ifPresent([&cursorPos] (auto& comp) {
        cursorPos -= comp.get();
   });*/

   //entityView.netForce += glm::vec2(deltaXForce, deltaYForce);
   //deltaXForce = 0;
   //deltaYForce = 0;
   //auto cursorPos = gameView.getCamera().getWorldPos(cursorScreenPos) - (glm::vec2)entityView.position;
   //float rot = atan2(cursorPos.y, cursorPos.x);

   /*entityView.get<component::Rotation2D>().ifPresent([rot] (component::Rotation2D& comp) {
       comp = rot;
   });*/

   entityView.get<common::GlobalTransform2D>().ifPresent([cursorPos] (common::GlobalTransform2D& comp) {
       auto disp = cursorPos - comp.transform2D.position();
       auto rot = atan2(disp.y, disp.x);
       comp.transform2D.setRotation(rot);
   });
}

void game::PlayerController::updateCursorPos (event::CursorPosChangeEvent &event) {
    cursorScreenPos = event.screenPos;
}

void game::PlayerController::updateDoShoot (event::PlayerShootChangeEvent &event) {
    doShoot = event.doShoot;
    hasShot &= doShoot;
}

/*int game::PlayerController::getTextureId (component::EntityView& entityView, view::GameView& gameView) const {
    return texId;
}

void game::PlayerController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test8");
}*/

void game::PlayerController::controlEntityPostPhysics (component::EntityView& entityView, view::GameView& gameView) {
    //auto pos = entityView.get<component::Position2D>().getUnsafe().get();
    auto pos = entityView.get<common::GlobalTransform2D>().getUnsafe().transform2D.position();
    if (!hasShot && doShoot) {
        //auto rot = entityView.get<component::Rotation2D>().getUnsafe().rotation;
        auto rot = entityView.get<common::GlobalTransform2D>().getUnsafe().transform2D.rotationAngle();
        glm::vec2 rotVec = glm::vec2{cos(rot), sin(rot)};
        glm::vec2 relPos = rotVec * SHOOT_DIST;
        glm::vec2 bulletVel = rotVec * SHOOT_VEL;

        util::DataObject bData;
        util::DataObject bVel;
        bVel["x"] = bulletVel.x;
        bVel["y"] = bulletVel.y;

        bData["momentum"] = bVel;

        /*auto bulletView = gameView.createEntityInstance("bullet_entity", pos.x + relPos.x,
                                                      pos.y + relPos.y, rot, util::DataValue(bData));*/
        auto bulletView = gameView.createEntityInstance("bullet_entity", util::DataValue(bData));
        /*bulletView.get<component::Position2D>().ifPresent([&pos, relPos] (component::Position2D& comp) {
            comp = pos + relPos;
        });*/
        /*bulletView.get<component::Rotation2D>().ifPresent([&rot] (component::Rotation2D& comp) {
            comp = rot;
        });*/
        bulletView.get<common::GlobalTransform2D>().ifPresent([rot, pos, relPos] (common::GlobalTransform2D& comp) {
           comp.transform2D
                .translate(pos + relPos)
                .setRotation(rot);
           comp.transform2D.setRotation(rot);
        });

        bulletView.get<physics::RigidBody2D>().ifPresent([bulletVel] (physics::RigidBody2D& comp) {
            comp.applyImpulse(comp.getMass() * bulletVel);
        });

        /*auto bulletView = entityView.withId(bulletId);
        //bulletView.rotation = rot(); // TODO: look into difference with ()
        bulletView.momentum = rotVec * SHOOT_VEL;*/
        hasShot = true;
    }
    gameView.getCamera().setPos(pos);
}

#define FORCE_COMPONENT 6.5

static glm::vec2 cursorPos{0.0f, 0.0f};

static common::InputAction KeyLeft;
static common::InputAction KeyRight;
static common::InputAction KeyUp;
static common::InputAction KeyDown;

static common::InputAction KeyShoot;

static void onCursorChange (event::CursorPosChangeEvent& event) {
    cursorPos = event.screenPos;
}

void addPlayerComponents (component::ComponentManager& manager, component::EntitySerialiser& serialiser) {
    manager.addComponent<Player>();

    serialiser.addComponentSerialiser<Player>("Player", [] (const Player& player) -> util::DataValue {
        return util::DataObject{};
    }, [] (const util::DataValue& val) -> util::Optional<Player> {
        if (val.is<util::DataObject>()) {
            return {Player{}};
        } else {
            return util::NullOpt;
        }
    });

}

void inputSetup (game::GameInput& input, const event::EventBus::SharedPtr& eventBus) {
    eventBus->subscribeUnscoped(onCursorChange);

    KeyLeft = input.getInput("move_left");
    KeyRight = input.getInput("move_right");
    KeyUp = input.getInput("move_up");
    KeyDown = input.getInput("move_down");

    KeyShoot = input.getInput("player_shoot");
}

void playerUpdate (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& object) {
    glm::vec2 forceVec{0.0f, 0.0f};

    if (input.isDown(KeyLeft)) {
        forceVec += glm::vec2{-FORCE_COMPONENT, 0.0f};
    }

    if (input.isDown(KeyRight)) {
        forceVec += glm::vec2{FORCE_COMPONENT, 0.0f};
    }

    if (input.isDown(KeyUp)) {
        forceVec += glm::vec2{0, FORCE_COMPONENT};
    }

    if (input.isDown(KeyDown)) {
        forceVec += glm::vec2{0, -FORCE_COMPONENT};
    }

    glm::vec2 worldCursorPos = object.getCamera().getWorldPos(cursorPos);

    manager.each<Player, common::GlobalTransform2D, physics::RigidBody2D>([forceVec, worldCursorPos] (component::IterInfo& info, Player& player, common::GlobalTransform2D& transform, physics::RigidBody2D& body) {
        body.applyForce(forceVec * body.getMass());

        auto disp = worldCursorPos - transform.transform2D.position();
        auto rot = atan2(disp.y, disp.x);
        transform.transform2D.setRotation(rot);
    });
}

void playerUpdatePost (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& game) {
    glm::vec2 worldCursorPos = game.getCamera().getWorldPos(cursorPos);
    bool doShoot = input.isDown(KeyShoot);

    manager.each<Player, common::GlobalTransform2D>([doShoot, &game] (component::IterInfo& info, Player& player, common::GlobalTransform2D& transform) {
        if (doShoot && !player.hasShot) {
            auto rot = transform.transform2D.rotationAngle();

            glm::vec2 rotVec = glm::vec2{cos(rot), sin(rot)};
            glm::vec2 pos = rotVec * SHOOT_DIST + transform.transform2D.position();
            glm::vec2 bulletVel = rotVec * SHOOT_VEL;

            auto bulletView = game.createNewEntityInstance("bullet_entity");

            bulletView.get<common::GlobalTransform2D>().ifPresent([pos, rot] (common::GlobalTransform2D& transform) {
                transform.transform2D
                    .setPosition(pos)
                    .setRotation(rot);
            });

            bulletView.get<physics::RigidBody2D>().ifPresent([bulletVel] (physics::RigidBody2D& body) {
                body.applyImpulse(bulletVel * body.getMass());
            });
        }
    });
}