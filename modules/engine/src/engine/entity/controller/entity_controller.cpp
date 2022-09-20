#include "engine/entity/controller/entity_controller.h"
#include "graphics/textures/texture_atlas.h"

#include <utility>
#include "logging/logging.h"
using namespace game;

void EntityController::controlEntityPrePhysics (component::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}


void EntityController::controlEntityPostPhysics (component::EntityView& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}

void EntityController::onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherView, unsigned int layers) {
    logging::log(LEVEL_DEBUG, "On entity collision!");
}

/*int EntityController::getTextureId (component::EntityView& entityView, view::GameView& gameView) const {
    return testTexId;
}

void EntityController::setTextureIds (graphics::TextureAtlas& atlas) {
    testTexId = atlas.getModelId("test3");
}*/

util::DataObject EntityController::getData (component::EntityView& entityView, view::GameView& gameView) {
    return util::DataObject();
}

EntityController::EntityController (std::string entityId) : entityId{std::move(entityId)} {}

const std::string& EntityController::getEntityId () const {
    return entityId;
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
        /*i.getComponent<std::shared_ptr<EntityController>>().ifPresent([&gameView, &i] (std::shared_ptr<EntityController>& ptr) {
            ptr->controlEntityPrePhysics(i, gameView);
        });*/

        i.getComponent<EntityControllerComponent>().ifPresent([&gameView, &i] (EntityControllerComponent& comp) {
            comp.get().controlEntityPrePhysics(i, gameView);
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
        /*i.getComponent<std::shared_ptr<EntityController>>().ifPresent([&i, &gameView] (std::shared_ptr<EntityController>& ptr) {
           ptr->controlEntityPostPhysics(i, gameView);
        });*/

        i.getComponent<EntityControllerComponent>().ifPresent([&i, &gameView] (EntityControllerComponent& comp) {
           comp.get().controlEntityPostPhysics(i, gameView);
        });
    }
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    //auto entityView = view::EntityView(view::ViewCore(collisionEvent.componentManager), collisionEvent.entityId, collisionEvent.eventBus);
    auto& firstEntity = collisionEvent.firstEntity;
    auto& secondEntity = collisionEvent.secondEntity;
    /*firstEntity.getComponent<std::shared_ptr<EntityController>>().ifPresent([&firstEntity, &secondEntity, &collisionEvent] (std::shared_ptr<EntityController>& ptr) {

        ptr->onEntityCollision(firstEntity, collisionEvent.gameView, secondEntity, collisionEvent.collisionLayers);
    });*/


    firstEntity.getComponent<EntityControllerComponent>().ifPresent([&firstEntity, &secondEntity, &collisionEvent] (EntityControllerComponent& comp) {
        comp.get().onEntityCollision(firstEntity, collisionEvent.gameView, secondEntity, collisionEvent.collisionLayers);
    });
    /*secondEntity.getComponent<std::shared_ptr<EntityController>>().ifPresent([&firstEntity, &secondEntity, &collisionEvent] (std::shared_ptr<EntityController>& ptr) {
        ptr->onEntityCollision(secondEntity, collisionEvent.gameView, firstEntity.getId(), collisionEvent.collisionLayers);
    });*/

    //entityView.controller()->onEntityCollision(entityView, collisionEvent.gameView, collisionEvent.otherId, collisionEvent.collisionLayers);
}

void game::addControlEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    eventBus->subscribeUnscoped(controlOnCollision);
}

EntityController& EntityControllerComponent::get () {
#ifndef NDEBUG
    if (!controller) {
        logging::log(LEVEL_FATAL, "Attempted to dereference null controller!");
        assert(false);
    }
#endif
    return *controller;
}

const EntityController& EntityControllerComponent::get () const {
#ifndef NDEBUG
    if (!controller) {
        logging::log(LEVEL_FATAL, "Attempted to dereference null controller!");
        assert(false);
    }
#endif
    return *controller;
}

EntityControllerComponent::EntityControllerComponent (EntityController* controller) : controller{controller} {}
