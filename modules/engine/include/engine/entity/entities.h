#pragma once

#include "engine/entity/entity.h"
#include "event/events/entity_register.h"

namespace game {
    void addEntities (event::EntityRegisterEvent& event);
}
