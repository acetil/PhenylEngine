#include "game_init.h"
#include "game_object.h"

#include "entity/entity_test.h"
#include "entity/entities.h"
#include "logging/logging.h"

using namespace game;
void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics);
GameObject* game::initGame (graphics::Graphics* graphics) {
    GameObject* gameObject = new GameObject();
    addEventHandlers(gameObject, graphics);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    event::EntityRegisterEvent* entityEvent = new event::EntityRegisterEvent(gameObject);
    gameObject->getEventBus()->raiseEvent(entityEvent);
    delete entityEvent;
    
    gameObject->setTextureIds(graphics);
    return gameObject;
}

void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
}

