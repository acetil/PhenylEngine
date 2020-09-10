#include "entity_controller.h"
#include "logging/logging.h"
using namespace game;

void EntityController::controlEntityPrePhysics (view::EntityView& entityView, event::EventBus* bus) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}


void EntityController::controlEntityPostPhysics (view::EntityView& entityView, event::EventBus* bus) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}

void EntityController::onEntityCollision (view::EntityView& entityView, int otherId, unsigned int layers) {
    logging::log(LEVEL_DEBUG, "On entity collision!");
}


void game::controlEntitiesPrePhysics (component::EntityComponentManager* manager, int startId, int numEntities,
                                      int direction, event::EventBus* bus) {
    auto viewCore = view::ViewCore(manager);
    for (int i = 0; i < numEntities; i++) {
        view::EntityView entityView = view::EntityView(viewCore, i);
        entityView.entity()->getController()->controlEntityPrePhysics(entityView, bus);
    }
}
void game::controlEntitiesPostPhysics(component::EntityComponentManager* manager, int startId, int numEntities,
                                      int direction, event::EventBus* bus) {
    auto viewCore = view::ViewCore(manager);
    for (int i = 0; i < numEntities; i++) {
        auto entityView = view::EntityView(viewCore, i);
        entityView.entity()->getController()->controlEntityPostPhysics(entityView, bus);
    }
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    auto entityView = view::EntityView(view::ViewCore(collisionEvent.componentManager), collisionEvent.entityId);
    entityView.entity()->getController()->onEntityCollision(entityView, collisionEvent.otherId, collisionEvent.collisionLayers);
}

void game::addControlEventHandlers (event::EventBus* eventBus) {
    eventBus->subscribeHandler(controlOnCollision);
}