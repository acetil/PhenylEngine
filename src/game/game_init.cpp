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
component::ComponentManager<AbstractEntity*>* getEntityComponentManager (event::EventBus* bus);
void registerTiles (GameObject* gameObject, graphics::Graphics* graphics);
GameObject* game::initGame (graphics::Graphics* graphics) {
    auto gameObject = new GameObject();
    addEventHandlers(gameObject, graphics);
    auto manager = getEntityComponentManager(gameObject->getEventBus());
    gameObject->setEntityComponentManager(manager);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    gameObject->getEventBus()->raiseEvent(event::EntityRegisterEvent(gameObject));
    logging::log(LEVEL_DEBUG, "Finished entity init!");
    gameObject->setTextureIds(graphics->getTextureAtlas("sprite").value());
    graphics->addEntityLayer(manager); // TODO: unhackify
    registerTiles(gameObject, graphics);
    logging::log(LEVEL_DEBUG, "Set texture ids!");
    return gameObject;
}

void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(physics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(graphics::Graphics::onEntityCreation, graphics);
}

component::ComponentManager<AbstractEntity*>* getEntityComponentManager (event::EventBus* bus) {
    auto manager = new component::ComponentManager<AbstractEntity*>(256, bus, "entity_ptr");
    manager->addComponent<component::EntityMainComponent>("main_component");
    manager->addComponent<float, 12>("uv");
    return manager;
}

void registerTiles (GameObject* gameObject, graphics::Graphics* graphics) {
    graphics::TextureAtlas atlas = graphics->getTextureAtlas("sprite").value();
    gameObject->registerTile(new Tile("test_tile1", atlas.getTextureId("test6"),
            atlas, 0.1, 0.1));
    gameObject->registerTile(new Tile("test_tile2", atlas.getTextureId("test7"),
                                      atlas, 0.1, 0.1));
}
