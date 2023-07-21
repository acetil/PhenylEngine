#include "engine/game_init.h"

#include <utility>
#include "engine/game_object.h"

//#include "engine/entity/entity_test.h"
#include "engine/entity/entities.h"
#include "component/component.h"
#include "physics/components/simple_friction.h"
#include "logging/logging.h"
#include "graphics/graphics_handlers.h"
#include "physics/physics.h"
#include "physics/components/2D/collision_component.h"
#include "engine/entity/controller/entity_controller.h"
#include "graphics/phenyl_graphics.h"
#include "engine/phenyl_game.h"

#include "graphics/ui/debug_ui.h"

using namespace game;
void addEventHandlers (game::PhenylGame gameObject, graphics::PhenylGraphics graphics, const event::EventBus::SharedPtr& eventBus);
component::EntityComponentManager getEntityComponentManager ();
void registerTiles (game::PhenylGame gameObject, const graphics::PhenylGraphics& graphics);

void game::initGame (const graphics::PhenylGraphics& graphics, game::PhenylGame gameObject, const event::EventBus::SharedPtr& eventBus) {
    //auto gameObject = detail::GameObject::NewSharedPtr();
    //addEventHandlers(gameObject, graphics, eventBus);
    //addControlEventHandlers(eventBus);
    //auto manager = getEntityComponentManager(eventBus);
    //gameObject.setEntityComponentManager(manager);
    logging::log(LEVEL_INFO, "Starting init of entities!");
    eventBus->raise(event::EntityRegisterEvent(gameObject));
    logging::log(LEVEL_DEBUG, "Finished entity init!");

    /*graphics.getTextureAtlas("sprite").ifPresent([&gameObject](auto& atlas){gameObject.setTextureIds(atlas);});
    graphics::addMapRenderLayer(graphics, eventBus);
    graphics.addEntityLayer(gameObject.c); // TODO: unhackify
    graphics.getUIManager().addRenderLayer(graphics.tempGetGraphics(), graphics.getRenderer());*/

    registerTiles(std::move(gameObject), graphics);
    //addEventHandlers(gameObject, graphics, eventBus);
    logging::log(LEVEL_DEBUG, "Set texture ids!");

    //graphics.setupWindowCallbacks(gameObject.getEventBus());

    //return gameObject;
}

void addEventHandlers (game::PhenylGame gameObject, graphics::PhenylGraphics graphics, const event::EventBus::SharedPtr& eventBus) {
    graphics.addEventHandlers(eventBus);
    gameObject.addEventHandlers(eventBus);

    eventBus->subscribeUnscoped(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    //eventBus->subscribeUnscoped(physics::onEntityCreation);
    //gameObject.getEventBus()->subscribeHandler(&graphics::detail::Graphics::onEntityCreation, graphics.tempGetGraphics());
    //eventBus->subscribeUnscoped(graphics::updateEntityRotation);
    //eventBus->subscribeUnscoped(physics::updateEntityHitboxRotation);

    graphics::addDebugEventHandlers(eventBus);
}

component::EntityComponentManager getEntityComponentManager () {
    auto manager = component::EntityComponentManager{256};
    /*manager->addComponent<component::SimpleFrictionMotion2D>("main_component");
    manager->addComponent<graphics::Model2D>("model");
    manager->insert<physics::CollisionComponent2D>("collision_component");
    manager->insert<graphics::GlobalTransform2D>("model_pos");*/
    return manager;
}

void registerTiles (game::PhenylGame gameObject, const graphics::PhenylGraphics& graphics) {
    graphics.getTextureAtlas("sprite").ifPresent([&gameObject](auto& atlas) {
        gameObject.registerTile(new Tile("test_tile1", atlas.getModelId("test6"),
                                          atlas, 0.1, 0.1));
        gameObject.registerTile(new Tile("test_tile2", atlas.getModelId("test7"),
                                          atlas, 0.1, 0.1));
    });
}
