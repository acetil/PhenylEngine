#include <string>
#include <utility>
#include <math.h>
#include <fstream>

#include "engine/game_object.h"
#include "logging/logging.h"
#include "physics/physics.h"
#include "common/events/entity_creation.h"
#include "engine/phenyl_game.h"
#include "common/assets/assets.h"

using namespace game;

detail::GameObject::~GameObject () {
}

event::EventBus::SharedPtr detail::GameObject::getEventBus () {
    return eventBus;
}
void detail::GameObject::setEntityComponentManager (component::EntityComponentManager* manager) {
    this->entityComponentManager = manager;
}

void detail::GameObject::updateCamera (graphics::Camera& _camera) {
    this->camera.updateCamera(_camera);
}

GameCamera& detail::GameObject::getCamera () {
    return camera;
}

void detail::GameObject::addEventHandlers (event::EventBus::SharedPtr _eventBus) {
    eventBus = std::move(_eventBus);
    gameInput.setEventBus(eventBus);
    eventScope = eventBus->getScope();
    //eventBus->subscribe(&detail::GameObject::mapReloadRequest, this, eventScope);
    //eventBus->subscribe(&detail::GameObject::mapDumpRequest, this, eventScope);
    //eventBus->subscribe(&detail::GameObject::mapLoadRequest, this, eventScope);
}

GameInput& detail::GameObject::getInput () {
    return gameInput;
}

void detail::GameObject::setSerializer (component::EntitySerializer* serializer) {
    this->serializer = serializer;
}

void detail::GameObject::addDefaultSerialisers () {}
