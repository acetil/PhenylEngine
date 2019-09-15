#include "entity_test.h"
#include "entity.h"
#include "logging/logging.h"

using namespace game;

game::EntityTest::EntityTest() {
    scale = 1.0f;
    name = "test_entity";
}

AbstractEntity* game::EntityTest::createEntity (float x, float y) {
    EntityTest* entity = new EntityTest ();
    entity->textureId = textureId;
    entity->scale = scale;
    entity->physicsBody = new physics::PhysicsBody(&entity->x, &entity->y, 100, 0, 800);
    entity->name = name;
    return entity;
}
void game::EntityTest::setTextureIds (graphics::Graphics* graphics) {
    textureId = graphics->getSpriteTextureId("test3");
    logging::logf(LEVEL_DEBUG, "EntityTest textureId: %d", textureId);
}