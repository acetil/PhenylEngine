#include "game/entity/controller/player_controller.h"
#include "entity_test.h"
#include "entity.h"
#include "controller/entity_controller.h"
#include "logging/logging.h"

using namespace game;

game::EntityTest::EntityTest() {
    scale = 0.1f;
    name = "test_entity";
    controller = new PlayerController();
}

AbstractEntity* game::EntityTest::createEntity () {
    EntityTest* entity = new EntityTest();
    entity->textureId = textureId;
    entity->scale = scale;
    entity->physicsBody = new physics::PhysicsBody(entity->x, entity->y, 100, 0, 800);
    entity->name = name;
    entity->controller = controller;
    if (entity == nullptr) {
        logging::log(LEVEL_ERROR, "Null test entity!");
    }
    return entity;
}
void game::EntityTest::setTextureIds (graphics::TextureAtlas& atlas) {
    //textureId = atlas.getTextureId("test3");
    textureId = atlas.getModelId("test3");
    logging::logf(LEVEL_DEBUG, "EntityTest textureId: %d", textureId);
}