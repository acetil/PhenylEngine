#include "entity_test.h"
#include "entity.h"
#include "controller/entity_controller.h"
#include "logging/logging.h"

using namespace game;

game::EntityTest::EntityTest() {
    scale = 0.1f;
    name = "test_entity";
    controller = new EntityController();
}

AbstractEntity* game::EntityTest::createEntity () {
    EntityTest* entity = new EntityTest();
    entity->textureId = textureId;
    entity->scale = scale;
    entity->physicsBody = new physics::PhysicsBody(entity->x, entity->y, 100, 0, 800);
    entity->name = name;
    entity->controller = new EntityController();
    if (entity == nullptr) {
        logging::log(LEVEL_ERROR, "Null test entity!");
    }
    return entity;
}
void game::EntityTest::setTextureIds (graphics::Graphics* graphics) {
    textureId = graphics->getSpriteTextureId("test3");
    logging::logf(LEVEL_DEBUG, "EntityTest textureId: %d", textureId);
}