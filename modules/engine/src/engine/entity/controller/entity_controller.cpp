#include "engine/entity/controller/entity_controller.h"

#include <utility>
#include "logging/logging.h"
using namespace game;

void EntityController::controlEntityPrePhysics (component::view::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}


void EntityController::controlEntityPostPhysics (component::view::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}

void EntityController::onEntityCollision (component::view::EntityView& entityView, view::GameView& gameView, component::EntityId otherId, unsigned int layers) {
    logging::log(LEVEL_DEBUG, "On entity collision!");
}

int EntityController::getTextureId (component::view::EntityView& entityView, view::GameView& gameView) const {
    return testTexId;
}

void EntityController::setTextureIds (graphics::TextureAtlas& atlas) {
    testTexId = atlas.getModelId("test3");
}

util::DataObject EntityController::getData (component::view::EntityView& entityView, view::GameView& gameView) {
    return util::DataObject();
}


/*void game::controlEntitiesPrePhysics (component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId, int numEntities,
                                      int direction, const event::EventBus::SharedPtr& bus) {
    auto entityIds = manager->getComponent<component::EntityId>().orElse(nullptr);
    //auto viewCore = view::ViewCore(std::move(manager));
    for (int i = 0; i < numEntities; i++) {
        auto entityView = manager->getEntityView(entityIds[i]);

        manager->getObjectData<std::shared_ptr<EntityController>>(entityIds[i]).ifPresent([&entityView, &gameView] (std::shared_ptr<EntityController>& ptr) {

            ptr->controlEntityPrePhysics(entityView, gameView);
        });

        //entityView.controller()->controlEntityPrePhysics(entityView, gameView);
    }
}*/

void game::controlEntitiesPrePhysics (const component::EntityComponentManager::SharedPtr& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus) {
    for (auto i : *manager) {
        i.getComponent<std::shared_ptr<EntityController>>().ifPresent([&gameView, &i] (std::shared_ptr<EntityController>& ptr) {
            ptr->controlEntityPrePhysics(i, gameView);
        });
    }
}

/*void game::controlEntitiesPostPhysics(component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId, int numEntities,
                                      int direction, const event::EventBus::SharedPtr& bus) {
    auto entityIds = manager->getComponent<component::EntityId>().orElse(nullptr);
    //auto viewCore = view::ViewCore(std::move(manager));
    for (int i = 0; i < numEntities; i++) {
        auto entityView = manager->getEntityView(entityIds[i]);

        manager->getObjectData<std::shared_ptr<EntityController>>(entityIds[i]).ifPresent([&entityView, &gameView] (std::shared_ptr<EntityController>& ptr) {

            ptr->controlEntityPostPhysics(entityView, gameView);
        });

        //entityView.controller()->controlEntityPostPhysics(entityView, gameView);
    }
}*/

void game::controlEntitiesPostPhysics (const component::EntityComponentManager::SharedPtr& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus) {
    for (auto i : *manager) {
        i.getComponent<std::shared_ptr<EntityController>>().ifPresent([&i, &gameView] (std::shared_ptr<EntityController>& ptr) {
           ptr->controlEntityPostPhysics(i, gameView);
        });
    }
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    //auto entityView = view::EntityView(view::ViewCore(collisionEvent.componentManager), collisionEvent.entityId, collisionEvent.eventBus);
    auto entityView = collisionEvent.componentManager->getEntityView(collisionEvent.entityId);
    collisionEvent.componentManager->getObjectData<std::shared_ptr<EntityController>>(collisionEvent.entityId).ifPresent([&entityView, &collisionEvent] (std::shared_ptr<EntityController>& ptr) {

        ptr->onEntityCollision(entityView, collisionEvent.gameView, collisionEvent.otherId, collisionEvent.collisionLayers);
    });

    //entityView.controller()->onEntityCollision(entityView, collisionEvent.gameView, collisionEvent.otherId, collisionEvent.collisionLayers);
}

void game::addControlEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    eventBus->subscribeUnscoped(controlOnCollision);
}