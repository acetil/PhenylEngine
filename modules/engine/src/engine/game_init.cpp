#include "engine/game_init.h"

#include <utility>
#include "engine/game_object.h"

#include "component/component.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "graphics/phenyl_graphics.h"
#include "engine/phenyl_game.h"

#include "graphics/ui/debug_ui.h"

using namespace game;
void addEventHandlers (game::PhenylGame gameObject, graphics::PhenylGraphics graphics, const event::EventBus::SharedPtr& eventBus);
component::EntityComponentManager getEntityComponentManager ();

void game::initGame (const graphics::PhenylGraphics& graphics, game::PhenylGame gameObject, const event::EventBus::SharedPtr& eventBus) {
}

void addEventHandlers (game::PhenylGame gameObject, graphics::PhenylGraphics graphics, const event::EventBus::SharedPtr& eventBus) {
    graphics.addEventHandlers(eventBus);
    gameObject.addEventHandlers(eventBus);

    //eventBus->subscribeUnscoped(game::addEntities);
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