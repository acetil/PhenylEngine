#include "game_init.h"
#include "game_object.h"

#include "entity/entity_test.h"
#include "entity/entities.h"
#include "component/component.h"
#include "component/main_component.h"
#include "logging/logging.h"
#include "graphics/graphics_handlers.h"
#include "physics/physics_new.h"

using namespace game;
void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics);
component::ComponentManager* getEntityComponentManager (event::EventBus* bus);
GameObject* game::initGame (graphics::Graphics* graphics) {
    GameObject* gameObject = new GameObject();
    gameObject->setGraphics(graphics);
    addEventHandlers(gameObject, graphics);
    gameObject->setEntityComponentManager(getEntityComponentManager(gameObject->getEventBus()));
    logging::log(LEVEL_INFO, "Starting init of entities!");
    event::EntityRegisterEvent* entityEvent = new event::EntityRegisterEvent(gameObject);
    gameObject->getEventBus()->raiseEvent(entityEvent);
    logging::log(LEVEL_DEBUG, "Finished entity init!");
    gameObject->setTextureIds(graphics);
    logging::log(LEVEL_DEBUG, "Set texture ids!");
    return gameObject;
}

void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
    gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(physics::onEntityCreation);
}

component::ComponentManager* getEntityComponentManager (event::EventBus* bus) {
    component::ComponentManager* manager = new component::ComponentManager(256, bus);
    manager->addComponent<component::EntityMainComponent>("main_component");
    manager->addComponent<float, 12>("uv");
    return manager;
}
