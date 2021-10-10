#include "game_init.h"

#include <utility>
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
void addEventHandlers (const GameObject::SharedPtr& gameObject, graphics::Graphics::SharedPtr graphics);
component::EntityComponentManager::SharedPtr getEntityComponentManager (event::EventBus::SharedPtr bus);
void registerTiles (const GameObject::SharedPtr& gameObject, const graphics::Graphics::SharedPtr& graphics);
GameObject::SharedPtr game::initGame (const graphics::Graphics::SharedPtr& graphics) {
    auto gameObject = GameObject::NewSharedPtr();
    addEventHandlers(gameObject, graphics);
    addControlEventHandlers(gameObject->getEventBus());
    auto manager = getEntityComponentManager(gameObject->getEventBus());
    gameObject->setEntityComponentManager(manager);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    gameObject->getEventBus()->raiseEvent(event::EntityRegisterEvent(gameObject));
    logging::log(LEVEL_DEBUG, "Finished entity init!");
    gameObject->setTextureIds(graphics->getTextureAtlas("sprite").value());
    graphics::addMapRenderLayer(graphics, gameObject->getEventBus());
    graphics->addEntityLayer(manager); // TODO: unhackify
    graphics->getUIManager().addRenderLayer(graphics, graphics->getRenderer());

    registerTiles(gameObject, graphics);
    logging::log(LEVEL_DEBUG, "Set texture ids!");

    graphics->setupWindowCallbacks(gameObject->getEventBus());

    return gameObject;
}

void addEventHandlers (const GameObject::SharedPtr& gameObject, graphics::Graphics::SharedPtr graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(physics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(&graphics::Graphics::onEntityCreation, std::move(graphics));
    gameObject->getEventBus()->subscribeHandler(graphics::updateEntityRotation);
    gameObject->getEventBus()->subscribeHandler(physics::updateEntityHitboxRotation);

    gameObject->getEventBus()->subscribeHandler(&GameObject::mapReloadRequest, gameObject);
    gameObject->getEventBus()->subscribeHandler(&GameObject::mapDumpRequest, gameObject);
    gameObject->getEventBus()->subscribeHandler(&GameObject::mapLoadRequest, gameObject);

    graphics::addDebugEventHandlers(gameObject->getEventBus());
}

component::EntityComponentManager::SharedPtr getEntityComponentManager (event::EventBus::SharedPtr bus) {
    auto manager = component::EntityComponentManager::NewSharedPtr(255, std::move(bus));
    /*manager->addComponent<component::EntityMainComponent>("main_component");
    manager->addComponent<graphics::FixedModel>("model");
    manager->addComponent<physics::CollisionComponent>("collision_component");
    manager->addComponent<graphics::AbsolutePosition>("model_pos");*/
    return manager;
}

void registerTiles (const GameObject::SharedPtr& gameObject, const graphics::Graphics::SharedPtr& graphics) {
    graphics::TextureAtlas atlas = graphics->getTextureAtlas("sprite").value();
    gameObject->registerTile(new Tile("test_tile1", atlas.getModelId("test6"),
            atlas, 0.1, 0.1));
    gameObject->registerTile(new Tile("test_tile2", atlas.getModelId("test7"),
                                      atlas, 0.1, 0.1));
}
