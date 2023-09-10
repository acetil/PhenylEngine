#include "component/view/game_view.h"
#include "engine/game_object.h"

using namespace view;

component::Entity GameView::createEntityInstance (const std::string& name) {
    return gameObject->createNewEntityInstance(name); // TODO
}

void GameView::destroyEntityInstance (component::EntityId entityId) {
    gameObject->deleteEntityInstance(entityId);
}

game::GameCamera& GameView::getCamera () {
    return gameObject->getCamera();
}

physics::IPhysics* GameView::getPhysics () {
    return physics;
}
