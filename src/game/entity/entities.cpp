#include <game/entity/controller/player_controller.h>
#include "entities.h"
#include "entity_test.h"
using namespace game;

void game::addEntities(event::EntityRegisterEvent& event) {
    auto entityTest = new EntityTest();
    // TODO: move to PlayerController
    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateMovement, (PlayerController*) entityTest->getController());
    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateCursorPos, (PlayerController*) entityTest->getController());
    event.gameObject->getEventBus()->subscribeHandler(&game::PlayerController::updateDoShoot, (PlayerController*) entityTest->getController());

    event.gameObject->registerEntity("test_entity", entityTest);
}