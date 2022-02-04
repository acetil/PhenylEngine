#include "engine/game_init.h"

#include <utility>
#include "engine/game_object.h"

#include "engine/entity/entity_test.h"
#include "engine/entity/entities.h"
#include "component/component.h"
#include "component/main_component.h"
#include "logging/logging.h"
#include "graphics/graphics_handlers.h"
#include "physics/physics.h"
#include "physics/collision_component.h"
#include "engine/entity/controller/entity_controller.h"
#include "graphics/graphics_update.h"
#include "graphics/phenyl_graphics.h"

#include "graphics/ui/debug_ui.h"

using namespace game;
void addEventHandlers (const GameObject::SharedPtr& gameObject, graphics::PhenylGraphics graphics);
component::EntityComponentManager::SharedPtr getEntityComponentManager (event::EventBus::SharedPtr bus);
void registerTiles (const GameObject::SharedPtr& gameObject, const graphics::PhenylGraphics& graphics);

GameObject::SharedPtr game::initGame (graphics::PhenylGraphics graphics) {
    auto gameObject = GameObject::NewSharedPtr();
    addEventHandlers(gameObject, graphics);
    addControlEventHandlers(gameObject->getEventBus());
    auto manager = getEntityComponentManager(gameObject->getEventBus());
    gameObject->setEntityComponentManager(manager);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    gameObject->getEventBus()->raiseEvent(event::EntityRegisterEvent(gameObject));
    logging::log(LEVEL_DEBUG, "Finished entity init!");

    graphics.getTextureAtlas("sprite").ifPresent([&gameObject](auto& atlas){gameObject->setTextureIds(atlas);});
    graphics::addMapRenderLayer(graphics, gameObject->getEventBus());
    graphics.addEntityLayer(manager); // TODO: unhackify
    graphics.getUIManager().addRenderLayer(graphics.tempGetGraphics(), graphics.getRenderer());

    registerTiles(gameObject, graphics);
    logging::log(LEVEL_DEBUG, "Set texture ids!");

    graphics.setupWindowCallbacks(gameObject->getEventBus());

    return gameObject;
}

void addEventHandlers (const GameObject::SharedPtr& gameObject, graphics::PhenylGraphics graphics) {
    gameObject->getEventBus()->subscribeHandler(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(physics::onEntityCreation);
    gameObject->getEventBus()->subscribeHandler(&graphics::detail::Graphics::onEntityCreation, graphics.tempGetGraphics());
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

void registerTiles (const GameObject::SharedPtr& gameObject, const graphics::PhenylGraphics& graphics) {
    graphics.getTextureAtlas("sprite").ifPresent([&gameObject](auto& atlas) {
        gameObject->registerTile(new Tile("test_tile1", atlas.getModelId("test6"),
                                          atlas, 0.1, 0.1));
        gameObject->registerTile(new Tile("test_tile2", atlas.getModelId("test7"),
                                          atlas, 0.1, 0.1));
    });
}
