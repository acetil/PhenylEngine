#include "engine/key_defaults.h"

#include <utility>
#include "event/event_bus.h"
#include "event/events/player_movement_change.h"
#include "event/events/player_shoot_change.h"

#include "util/debug_console.h"

#define FORCE_COMPONENT 0.01
using namespace game;
class KeyMovement : public KeyboardFunction {
    private:
    event::EventBus::WeakPtr bus;
    float xComponent;
    float yComponent;
    bool isDown;
    public:
    KeyMovement(event::EventBus::WeakPtr bus, float xComponent, float yComponent);
    void operator() (int action) override;
};
class MouseShoot : public MouseFunction {
private:
    event::EventBus::WeakPtr bus;
    bool isDown;
public:
    explicit MouseShoot (event::EventBus::WeakPtr bus) {
        this->bus = std::move(bus);
        isDown = false;
    };
    void operator() (int action, glm::vec2 screenPos, glm::vec2 worldPos) override {
        if (!isDown && action == GLFW_PRESS) {
            isDown = true;
            if (!bus.expired()) {
                bus.lock()->raiseEvent(event::PlayerShootChangeEvent(true));
            }
        } else if (isDown && action == GLFW_RELEASE) {
            isDown = false;
            if (!bus.expired()) {
                bus.lock()->raiseEvent(event::PlayerShootChangeEvent(false));
            }
        }
    };
};

class DebugKey : public KeyboardFunction {
private:
    event::EventBus::WeakPtr bus;
    bool isDown;
public:
    explicit DebugKey (event::EventBus::WeakPtr _bus) : bus(std::move(_bus)), isDown(false) {}
    void operator() (int action) override {
        if (!isDown && action == GLFW_PRESS) {
            isDown = true;
        } else if (isDown && action == GLFW_RELEASE) {
            isDown = false;
            if (!bus.expired()) {
                util::doDebugConsole(bus.lock());
            }
        }
    }
};

void game::setupMovementKeys (const KeyboardInput::SharedPtr& keyInput, const event::EventBus::SharedPtr& bus) {
    setupKeyboardInputListeners(keyInput, bus);
    keyInput->setKey(GLFW_KEY_W, std::make_unique<KeyMovement>(bus, 0, FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_A, std::make_unique<KeyMovement>(bus, -1 * FORCE_COMPONENT, 0));
    keyInput->setKey(GLFW_KEY_S, std::make_unique<KeyMovement>(bus, 0, -1 * FORCE_COMPONENT));
    keyInput->setKey(GLFW_KEY_D, std::make_unique<KeyMovement>(bus, FORCE_COMPONENT, 0));

    keyInput->setMouseButton(GLFW_MOUSE_BUTTON_LEFT, std::make_unique<MouseShoot>(bus));

    keyInput->setKey(GLFW_KEY_F12, std::make_unique<DebugKey>(bus));
}
// TODO: move to vectors
KeyMovement::KeyMovement(event::EventBus::WeakPtr bus, float xComponent, float yComponent) {
    this->bus = bus;
    this->xComponent = xComponent;
    this->yComponent = yComponent;
    this->isDown = false;
}
void KeyMovement::operator() (int action) {
    if (!isDown && action == GLFW_PRESS) {
        if (!bus.expired()) {
            bus.lock()->raiseEvent(event::PlayerMovementChangeEvent(xComponent, yComponent));
        }
        isDown = true;
    } else if (isDown && action == GLFW_RELEASE) {
        if (!bus.expired()) {
            bus.lock()->raiseEvent(event::PlayerMovementChangeEvent(-1 * xComponent, -1 * yComponent));
        }
        isDown = false;
    }
}