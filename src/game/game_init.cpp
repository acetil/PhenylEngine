#include "game_init.h"
#include "game_object.h"

#include "entity/entity_test.h"
#include "entity/entities.h"
#include "component/component.h"
#include "component/main_component.h"
#include "logging/logging.h"

using namespace game;
void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics);
component::ComponentManager* getEntityComponentManager (event::EventBus* bus);
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

component::ComponentManager* getEntityComponentManager (event::EventBus* bus) {
    component::ComponentManager* manager = new component::ComponentManager(256, bus);
    manager->addComponent<component::EntityMainComponent>("main_component");
    return manager;
}
