#include "entities.h"
#include "entity_test.h"
using namespace game;

void game::addEntities(event::EntityRegisterEvent* event) {
    event->gameObject->registerEntity("test_entity", new EntityTest());
}