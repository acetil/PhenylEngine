#include "key_defaults.h"
#include "physics/physics.h"

#define FORCE_COMPONENT 1
#define FORWARD_TAG 1
#define BACK_TAG 2
#define LEFT_TAG 3
#define RIGHT_TAG 4
using namespace game;
class KeyMovement : public KeyboardFunction {
    private:
    AbstractEntity** playerPtr;
    float xComponent;
    float yComponent;
    int tag;
    bool isDown;
    public:
    KeyMovement(AbstractEntity** playerPtr, float xComponent, float yComponent, int tag);
    virtual void operator() (int action);
};
void game::setupMovementKeys (KeyboardInput* keyInput, AbstractEntity** playerPtr) {
    keyInput->setKey(GLFW_KEY_W, new KeyMovement(playerPtr, 0, FORCE_COMPONENT, FORWARD_TAG));
    keyInput->setKey(GLFW_KEY_A, new KeyMovement(playerPtr, -1 * FORCE_COMPONENT, 0, LEFT_TAG));
    keyInput->setKey(GLFW_KEY_S, new KeyMovement(playerPtr, 0, -1 * FORCE_COMPONENT, BACK_TAG));
    keyInput->setKey(GLFW_KEY_D, new KeyMovement(playerPtr, FORCE_COMPONENT, 0, RIGHT_TAG));
}
KeyMovement::KeyMovement(AbstractEntity** playerPtr, float xComponent, float yComponent, int tag) {
    this->playerPtr = playerPtr;
    this->xComponent = xComponent;
    this->yComponent = yComponent;
    this->tag = tag;
    this->isDown = false;
}
void KeyMovement::operator() (int action) {
    if (*playerPtr != NULL) {
        if (!isDown && action == GLFW_PRESS) {
            (*playerPtr)->addForce(physics::Force(xComponent, yComponent, CONSTANT_DEGREE, MOVEMENT_FORCE_TYPE, tag, 0));
            isDown = true;
        } else if (isDown && action == GLFW_RELEASE) {
            (*playerPtr)->clearMovementForces(tag);
            isDown = false;
        }
    }
}