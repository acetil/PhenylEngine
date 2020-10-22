#include "entity_controller.h"
#include "logging/logging.h"
using namespace game;

void EntityController::controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}


void EntityController::controlEntityPostPhysics (view::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}

void EntityController::onEntityCollision (view::EntityView& entityView, view::GameView& gameView, int otherId, unsigned int layers) {
    logging::log(LEVEL_DEBUG, "On entity collision!");
}

int EntityController::getTextureId (view::EntityView& entityView, view::GameView& gameView) {
    return testTexId;
}

void EntityController::setTextureIds (graphics::TextureAtlas& atlas) {
    testTexId = atlas.getModelId("test3");
}


void game::controlEntitiesPrePhysics (component::EntityComponentManager* manager, view::GameView& gameView, int startId, int numEntities,
                                      int direction, event::EventBus* bus) {
    auto viewCore = view::ViewCore(manager);
    for (int i = 0; i < numEntities; i++) {
        view::EntityView entityView = view::EntityView(viewCore, i, bus);
        entityView.controller()->controlEntityPrePhysics(entityView, gameView);
    }
}
void game::controlEntitiesPostPhysics(component::EntityComponentManager* manager, view::GameView& gameView, int startId, int numEntities,
                                      int direction, event::EventBus* bus) {
    auto viewCore = view::ViewCore(manager);
    for (int i = 0; i < numEntities; i++) {
        auto entityView = view::EntityView(viewCore, i, bus);
        entityView.controller()->controlEntityPostPhysics(entityView, gameView);
    }
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    auto entityView = view::EntityView(view::ViewCore(collisionEvent.componentManager), collisionEvent.entityId, collisionEvent.eventBus);
    entityView.controller()->onEntityCollision(entityView, collisionEvent.gameView, collisionEvent.otherId, collisionEvent.collisionLayers);
}

void game::addControlEventHandlers (event::EventBus* eventBus) {
    eventBus->subscribeHandler(controlOnCollision);
}