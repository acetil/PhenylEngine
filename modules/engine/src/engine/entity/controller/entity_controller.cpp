#include "engine/entity/controller/entity_controller.h"
#include "graphics/textures/texture_atlas.h"

#include <utility>
#include "logging/logging.h"
using namespace game;

void EntityController::controlEntityPrePhysics (component::Entity& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}


void EntityController::controlEntityPostPhysics (component::Entity& entityView, view::GameView& gameView) {
    //logging::log(LEVEL_INFO, "Controlling entity pre physics!");
}

void EntityController::onEntityCollision (component::Entity& entityView, view::GameView& gameView, component::Entity& otherView, unsigned int layers) {
    //logging::log(LEVEL_DEBUG, "On entity collision!");
}

util::DataObject EntityController::getData (component::Entity& entityView, view::GameView& gameView) {
    return util::DataObject();
}

EntityController::EntityController (std::string entityId) : entityId{std::move(entityId)} {}

const std::string& EntityController::getEntityId () const {
    return entityId;
}


void game::controlEntitiesPrePhysics (component::EntityComponentManager& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus) {
    /*for (auto i : manager) {
        i.get<EntityControllerComponent>().ifPresent([&gameView, &i] (EntityControllerComponent& comp) {
            comp.get().controlEntityPrePhysics(i, gameView);
        });
    }*/

    /*for (auto [id, controller] : manager.iterate<EntityControllerComponent>().withId()) {
        auto view = manager.entity(id);
        controller.get().controlEntityPrePhysics(entity, gameView);
    }*/
    /*manager.each<EntityControllerComponent>([&gameView] (component::IterInfo& info, EntityControllerComponent& controller) {
        auto view = info.manager().entity(info.id());
        controller.get().controlEntityPrePhysics(entity, gameView);
    });*/
}

void game::controlEntitiesPostPhysics (component::EntityComponentManager& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus) {
    /*for (auto [id, controller] : manager.iterate<EntityControllerComponent>().withId()) {
        auto view = manager.entity(id);
        controller.get().controlEntityPostPhysics(entity, gameView);
    }*/
    /*manager.each<EntityControllerComponent>([&gameView] (component::IterInfo& info, EntityControllerComponent& controller) {
        auto entity = info.manager().view(info.id());
        controller.get().controlEntityPostPhysics(entity, gameView);
    });*/
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    auto& firstEntity = collisionEvent.firstEntity;
    auto& secondEntity = collisionEvent.secondEntity;

    firstEntity.get<EntityControllerComponent>().ifPresent([&firstEntity, &secondEntity, &collisionEvent] (EntityControllerComponent& comp) {
        comp.get().onEntityCollision(firstEntity, collisionEvent.gameView, secondEntity, collisionEvent.collisionLayers);
    });
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
