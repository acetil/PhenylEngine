#include "key_input.h"
#include "entity/entity.h"
#ifndef KEY_DEFAULTS_H
#define KEY_DEFAULTS_H
namespace game {
    void setupMovementKeys (KeyboardInput* keyInput, AbstractEntity** playerPtr);
}
#endif