#pragma once

#include "key_input.h"
#include "engine/entity/entity.h"
#include "event/event_bus.h"

namespace game {
    void setupMovementKeys (const KeyboardInput::SharedPtr& keyInput, const event::EventBus::SharedPtr& playerPtr);
}
