#include "entity.h"
#include "event/events.h"
#ifndef ENTITIES_H
#define ENTITIES_H
namespace game {
    void addEntities (event::EntityRegisterEvent* event);
}
#endif