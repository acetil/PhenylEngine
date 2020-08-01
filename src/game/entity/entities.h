#include "entity.h"
#include "event/events/entity_register.h"
#ifndef ENTITIES_H
#define ENTITIES_H
namespace game {
    void addEntities (event::EntityRegisterEvent& event);
}
#endif