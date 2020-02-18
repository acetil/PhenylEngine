#include "key_input.h"
#include "entity/entity.h"
#include "event/event.h"
#ifndef KEY_DEFAULTS_H
#define KEY_DEFAULTS_H
namespace game {
    void setupMovementKeys (KeyboardInput* keyInput, event::EventBus* playerPtr);
}
#endif