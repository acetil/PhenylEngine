#include <stdexcept>

#include "engine/phenyl_game.h"
#include "graphics/phenyl_graphics.h"
#include "engine/game_object.h"
#include "engine/game_init.h"

using namespace game;

std::shared_ptr<game::detail::GameObject> PhenylGame::getShared () const {
    auto ptr = gameObject.lock();

#ifndef NDEBUG
    if (!ptr) {
        logging::log(LEVEL_FATAL, "Game object attempted to be accessed after it was deleted!");
        throw std::runtime_error("Game object attempted to be accessed after it was deleted!");
    }
#endif
    return ptr;
}


void PhenylGame::setEntityComponentManager (component::EntityComponentManager* compManager) {
    getShared()->setEntityComponentManager(compManager);
}

event::EventBus::SharedPtr PhenylGame::getEventBus () {
    return getShared()->getEventBus();
}

void PhenylGame::updateCamera (graphics::Camera& camera) {
    getShared()->updateCamera(camera);
}

GameCamera& PhenylGame::getCamera () {
    return getShared()->getCamera();
}

void PhenylGame::addEventHandlers (event::EventBus::SharedPtr eventBus) {
    getShared()->addEventHandlers(std::move(eventBus));
}

GameInput& PhenylGame::getGameInput () {
    return getShared()->getInput();
}

void PhenylGame::addDefaultSerialisers () {
    getShared()->addDefaultSerialisers();
}

void PhenylGame::setSerializer (component::EntitySerializer* serialiser) {
    getShared()->setSerializer(serialiser);
}

PhenylGameHolder::~PhenylGameHolder () = default;

PhenylGame PhenylGameHolder::getGameObject () const {
    return PhenylGame(gameObject);
}

PhenylGameHolder::PhenylGameHolder () {
    gameObject = detail::GameObject::NewSharedPtr();
}

void PhenylGameHolder::initGame (const graphics::PhenylGraphics& graphics, event::EventBus::SharedPtr eventBus) {
    game::initGame(graphics, getGameObject(), eventBus);
}
