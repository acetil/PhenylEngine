#include "entity_test.h"
#include "entity.h"
using namespace game;

game::EntityTest::EntityTest() {
    scale = 0.5f;
    physicsBody = nullptr;
    name = "test_entity";
}

AbstractEntity* game::EntityTest::createEntity (float x, float y) {
    EntityTest* entity = new EntityTest ();
    entity->textureId = textureId;
    entity->scale = scale;
    physicsBody = nullptr;
    entity->name = name;
    return entity;
}
void game::EntityTest::setTextureIds (graphics::TextureAtlas* atlas) {
    textureId = atlas->getTextureId("test_img");
}