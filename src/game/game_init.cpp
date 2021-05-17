#include "game_init.h"
#include "game_object.h"

#include "entity/entity_test.h"
#include "entity/entities.h"
#include "component/component.h"
#include "component/main_component.h"
#include "logging/logging.h"
#include "graphics/graphics_handlers.h"
#include "physics/physics.h"
#include "physics/collision_component.h"
#include "game/entity/controller/entity_controller.h"
#include "graphics/graphics_update.h"

#include "graphics/ui/debug_ui.h"

using namespace game;
void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics);
component::EntityComponentManager* getEntityComponentManager (event::EventBus* bus);
void registerTiles (GameObject* gameObject, graphics::Graphics* graphics);
GameObject* game::initGame (graphics::Graphics* graphics) {
    auto gameObject = new GameObject();
    addEventHandlers(gameObject, graphics);
    addControlEventHandlers(gameObject->getEventBus());
    auto manager = getEntityComponentManager(gameObject->getEventBus());
    gameObject->setEntityComponentManager(manager);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    gameObject->getEventBus()->raiseEvent(event::EntityRegisterEvent(gameObject));
    logging::log(LEVEL_DEBUG, "Finished entity init!");
    gameObject->setTextureIds(graphics->getTextureAtlas("sprite").value());
    graphics->addEntityLayer(manager); // TODO: unhackify
    graphics->getUIManager().addRenderLayer(graphics, graphics->getRenderer());

    registerTiles(gameObject, graphics);
    logging::log(LEVEL_DEBUG, "Set texture ids!");

    graphics->setupWindowCallbacks(gameObject->getEventBus());

    return gameObject;
}

void addEventHandlers (GameObject* gameObject, graphics::Graphics* graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(physics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(&graphics::Graphics::onEntityCreation, graphics);
    gameObject->getEventBus()->subscribeHandler(graphics::updateEntityRotation);
    gameObject->getEventBus()->subscribeHandler(physics::updateEntityHitboxRotation);

    graphics::addDebugEventHandlers(gameObject->getEventBus());
}

component::EntityComponentManager* getEntityComponentManager (event::EventBus* bus) {
    auto manager = new component::EntityComponentManager(255, bus);
    /*manager->addComponent<component::EntityMainComponent>("main_component");
    manager->addComponent<graphics::FixedModel>("model");
    manager->addComponent<physics::CollisionComponent>("collision_component");
    manager->addComponent<graphics::AbsolutePosition>("model_pos");*/
    return manager;
}

void registerTiles (GameObject* gameObject, graphics::Graphics* graphics) {
    graphics::TextureAtlas atlas = graphics->getTextureAtlas("sprite").value();
    gameObject->registerTile(new Tile("test_tile1", atlas.getModelId("test6"),
            atlas, 0.1, 0.1));
    gameObject->registerTile(new Tile("test_tile2", atlas.getModelId("test7"),
                                      atlas, 0.1, 0.1));
}
