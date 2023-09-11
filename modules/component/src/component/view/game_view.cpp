#include "component/view/game_view.h"
#include "engine/game_object.h"

using namespace view;

game::GameCamera& GameView::getCamera () {
    return gameObject->getCamera();
}

physics::IPhysics* GameView::getPhysics () {
    return physics;
}
