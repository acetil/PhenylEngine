#include "engine/entity/controller/player_controller.h"
#include "engine/entity/entities.h"
//#include "engine/entity/entity_test.h"
//#include "engine/entity/entity_bullet.h"
//#include "basic_entity.h"
#include "engine/entity/controller/bullet_controller.h"
#include "engine/game_object.h"
using namespace game;

void game::addEntities(event::EntityRegisterEvent& event) {
    //auto entityTest = new EntityTest();
    //auto entityBullet = new EntityBullet();

    /*auto entityTestType = EntityTypeBuilder("test_entity", "test_entity", constructor_factory<AbstractEntity, EntityTest>)
            .setMass(10.0f)
            .setConstFriction(0.005)
            .setLinearFriction(0.27)
            .setScale(0.1f)
            .addCollisionLayers(1 | 2)
            .addEventLayers(1)
            .addLayers(1)
            .addResolveLayers(1 | 2);

    auto entityBulletType = EntityTypeBuilder("bullet", "bullet", constructor_factory<AbstractEntity, EntityBullet>)
            .setScale(0.03f)
            .setMass(1.0f)
            .setConstFriction(0.0001)
            .setLinearFriction(0.01)
            .addCollisionLayers(1 | 2)
            .addEventLayers(1 | 2)
            .addLayers(1);

    auto entityWallType = EntityTypeBuilder("wall_entity", "wall_entity", constructor_factory<AbstractEntity, BasicEntity>)
            .setScale(0.1f)
            .setMass(0.0f)
            .addLayers(2);


    event.gameObject.registerEntityType("test_entity", entityTestType);
    event.gameObject.registerEntityType("bullet", entityBulletType);
    event.gameObject.registerEntityType("wall_entity", entityWallType);*/

    auto playerController = std::make_unique<PlayerController>();

    playerController->getEventScope() = event.gameObject.getEventBus()->getScope();

    event.gameObject.getEventBus()->subscribe(&game::PlayerController::updateMovement, playerController.get(),
                                              playerController->getEventScope());
    event.gameObject.getEventBus()->subscribe(&game::PlayerController::updateCursorPos, playerController.get(),
                                              playerController->getEventScope());
    event.gameObject.getEventBus()->subscribe(&game::PlayerController::updateDoShoot, playerController.get(),
                                              playerController->getEventScope());

    //event.gameObject.tempGetPtr()->registerEntityController<PlayerController>("test_entity");
    event.gameObject.tempGetPtr()->registerEntityController(std::move(playerController));
    //event.gameObject.tempGetPtr()->registerEntityController<BulletController>("bullet");
    event.gameObject.tempGetPtr()->registerEntityController(std::make_unique<BulletController>());
    //event.gameObject.tempGetPtr()->registerEntityController<WallController>("wall_entity");
    //event.gameObject.tempGetPtr()->registerEntityController(std::make_unique<WallController>());

    //event.gameObject.buildEntityTypes();

    //auto playerController = std::dynamic_pointer_cast<game::PlayerController>(event.gameObject.getController("test_entity"));

    //event.gameObject->registerEntity("test_entity", entityTest);
    //event.gameObject->registerEntity("bullet", entityBullet);
}