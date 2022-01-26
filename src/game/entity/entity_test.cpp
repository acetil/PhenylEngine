#include "engine/entity/controller/player_controller.h"
#include "engine/entity/entity_test.h"
#include "engine/entity/entity.h"
#include "engine/entity/controller/entity_controller.h"
#include "logging/logging.h"

using namespace game;

game::EntityTest::EntityTest() {
    scale = 0.1f;
    name = "test_entity";
    //controller = new PlayerController(); //TODO: move somewhere else
}

AbstractEntity* game::EntityTest::createEntity () {
    EntityTest* entity = new EntityTest();
    entity->textureId = textureId;
    entity->scale = scale;
    entity->name = name;
    //entity->controller = controller;
    return entity;
}
void game::EntityTest::setTextureIds (graphics::TextureAtlas& atlas) {
    //textureId = atlas.getTextureId("test3");
    textureId = atlas.getModelId("test8");
    logging::log(LEVEL_DEBUG, "EntityTest textureId: {}", textureId);
}