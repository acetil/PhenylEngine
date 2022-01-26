#include "key_input.h"
#include "engine/entity/entity.h"
#include "event/event_bus.h"
#ifndef KEY_DEFAULTS_H
#define KEY_DEFAULTS_H
namespace game {
    void setupMovementKeys (const KeyboardInput::SharedPtr& keyInput, const event::EventBus::SharedPtr& playerPtr);
}
#endif