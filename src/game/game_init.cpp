#include "game_init.h"
#include "game_object.h"

#include "entity/entity_test.h"

#include "logging/logging.h"

using namespace game;

GameObject* game::initGame (graphics::Graphics* graphics) {
    GameObject* gameObject = new GameObject();

    logging::log(LEVEL_INFO, "Starting init of entities!");
    gameObject->registerEntity("test_entity", new EntityTest());

    gameObject->setTextureIds(graphics);
    return gameObject;
}