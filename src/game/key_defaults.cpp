#include "key_defaults.h"
#include "physics/physics.h"
#include "event/event.h"
#include "event/events/player_movement_change.h"

#define FORCE_COMPONENT 0.01
#define FORWARD_TAG 1
#define BACK_TAG 2
#define LEFT_TAG 3
#define RIGHT_TAG 4
using namespace game;
class KeyMovement : public KeyboardFunction {
    private:
    event::EventBus* bus;
    float xComponent;
    float yComponent;
    bool isDown;
    public:
    KeyMovement(event::EventBus* bus, float xComponent, float yComponent);
    void operator() (int action) override;
};
void game::setupMovementKeys (KeyboardInput* keyInput, event::EventBus* bus) {
    keyInput->setKey(GLFW_KEY_W, new KeyMovement(bus, 0, FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_A, new KeyMovement(bus, -1 * FORCE_COMPONENT, 0));
    keyInput->setKey(GLFW_KEY_S, new KeyMovement(bus, 0, -1 * FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_D, new KeyMovement(bus, FORCE_COMPONENT, 0));
}
KeyMovement::KeyMovement(event::EventBus* bus, float xComponent, float yComponent) {
    this->bus = bus;
    this->xComponent = xComponent;
    this->yComponent = yComponent;
    this->isDown = false;
}
void KeyMovement::operator() (int action) {
    if (!isDown && action == GLFW_PRESS) {
        bus->raiseEvent(new event::PlayerMovementChangeEvent(xComponent, yComponent));
        isDown = true;
    } else if (isDown && action == GLFW_RELEASE) {
        bus->raiseEvent(new event::PlayerMovementChangeEvent(-1 * xComponent, -1 * yComponent));
        isDown = false;
    }
}