#include <game/entity/controller/player_controller.h>
#include "entities.h"
#include "entity_test.h"
#include "entity_bullet.h"
#include "basic_entity.h"
#include "entity_type.h"
#include "controller/bullet_controller.h"
#include "controller/wall_controller.h"
using namespace game;

void game::addEntities(event::EntityRegisterEvent& event) {
    //auto entityTest = new EntityTest();
    //auto entityBullet = new EntityBullet();

    auto entityTestType = EntityTypeBuilder("test_entity", constructor_factory<AbstractEntity, EntityTest>)
            .setMass(10.0f)
            .setConstFriction(0.005)
            .setLinearFriction(0.27)
            .setScale(0.1f)
            .addCollisionLayers(1 | 2)
            .addEventLayers(1)
            .addLayers(1)
            .addResolveLayers(1 | 2);

    auto entityBulletType = EntityTypeBuilder("bullet", constructor_factory<AbstractEntity, EntityBullet>)
            .setScale(0.03f)
            .setMass(1.0f)
            .setConstFriction(0.0001)
            .setLinearFriction(0.01)
            .addCollisionLayers(1 | 2)
            .addEventLayers(1 | 2)
            .addLayers(1);

    auto entityWallType = EntityTypeBuilder("wall_entity", constructor_factory<AbstractEntity, BasicEntity>)
            .setScale(0.1f)
            .setMass(0.0f)
            .addLayers(2);


    event.gameObject->registerEntityType("test_entity", entityTestType);
    event.gameObject->registerEntityType("bullet", entityBulletType);
    event.gameObject->registerEntityType("wall_entity", entityWallType);
    event.gameObject->registerEntityController<PlayerController>("test_entity");
    event.gameObject->registerEntityController<BulletController>("bullet");
    event.gameObject->registerEntityController<WallController>("wall_entity");

    event.gameObject->buildEntityTypes();

    auto playerController = std::dynamic_pointer_cast<game::PlayerController>(event.gameObject->getController("test_entity"));

    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateMovement, playerController);
    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateCursorPos, playerController);
    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateDoShoot, playerController);

    //event.gameObject->registerEntity("test_entity", entityTest);
    //event.gameObject->registerEntity("bullet", entityBullet);
}