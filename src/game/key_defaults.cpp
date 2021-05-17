#include "key_defaults.h"
#include "event/event.h"
#include "event/events/player_movement_change.h"
#include "event/events/player_shoot_change.h"

#include "util/debug_console.h"

#define FORCE_COMPONENT 0.01
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
class MouseShoot : public MouseFunction {
private:
    event::EventBus* bus;
    bool isDown;
public:
    explicit MouseShoot (event::EventBus* bus) {
        this->bus = bus;
        isDown = false;
    };
    void operator() (int action, glm::vec2 screenPos, glm::vec2 worldPos) override {
        if (!isDown && action == GLFW_PRESS) {
            isDown = true;
            bus->raiseEvent(event::PlayerShootChangeEvent(true));
        } else if (isDown && action == GLFW_RELEASE) {
            isDown = false;
            bus->raiseEvent(event::PlayerShootChangeEvent(false));
        }
    };
};

class DebugKey : public KeyboardFunction {
private:
    event::EventBus* bus;
    bool isDown;
public:
    explicit DebugKey (event::EventBus* _bus) : bus(_bus), isDown(false) {}
    void operator() (int action) override {
        if (!isDown && action == GLFW_PRESS) {
            isDown = true;
        } else if (isDown && action == GLFW_RELEASE) {
            isDown = false;
            util::doDebugConsole(bus);
        }
    }
};

void game::setupMovementKeys (KeyboardInput* keyInput, event::EventBus* bus) {
    setupKeyboardInputListeners(keyInput, bus);
    keyInput->setKey(GLFW_KEY_W, new KeyMovement(bus, 0, FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_A, new KeyMovement(bus, -1 * FORCE_COMPONENT, 0));
    keyInput->setKey(GLFW_KEY_S, new KeyMovement(bus, 0, -1 * FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_D, new KeyMovement(bus, FORCE_COMPONENT, 0));

    keyInput->setMouseButton(GLFW_MOUSE_BUTTON_LEFT, new MouseShoot(bus));

    keyInput->setKey(GLFW_KEY_F12, new DebugKey(bus));
}
// TODO: move to vectors
KeyMovement::KeyMovement(event::EventBus* bus, float xComponent, float yComponent) {
    this->bus = bus;
    this->xComponent = xComponent;
    this->yComponent = yComponent;
    this->isDown = false;
}
void KeyMovement::operator() (int action) {
    if (!isDown && action == GLFW_PRESS) {
        bus->raiseEvent(event::PlayerMovementChangeEvent(xComponent, yComponent));
        isDown = true;
    } else if (isDown && action == GLFW_RELEASE) {
        bus->raiseEvent(event::PlayerMovementChangeEvent(-1 * xComponent, -1 * yComponent));
        isDown = false;
    }
}