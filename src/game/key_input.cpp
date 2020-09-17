#include <vector>
#include <utility>

#include "key_input.h"
#include "graphics/graphics_headers.h"
#include "graphics/renderers/glrenderer.h"

#include "event/events/cursor_position_change.h"

using namespace game;
// TODO: add this to renderer
namespace game {
    class KeyboardInputImpl : public KeyboardInput {
    private:
        GLFWwindow* window;
        std::map<int, KeyboardFunction*> bindingMap;
        std::map<int, MouseFunction*> mouseBindingMap;
        glm::vec2 screenMousePos;
        glm::vec2 worldMousePos;
    public:
        explicit KeyboardInputImpl (graphics::Graphics* graphics);

        ~KeyboardInputImpl ();

        void setKey (int key, KeyboardFunction* function) override;

        void setMouseButton (int button, MouseFunction* function) override;

        void replaceKey (int after, int before) override;

        void handleKeyPresses () override;

        void onCursorPosChange (event::CursorPosChangeEvent& event);
    };
}

game::KeyboardInputImpl::KeyboardInputImpl (graphics::Graphics* graphics) {

    window = ((graphics::GLRenderer*)graphics->getRenderer())->getWindow();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    // TODO: add key press preference reading here
}

void game::KeyboardInputImpl::handleKeyPresses () {
    for (auto const& x : bindingMap) {
        x.second->operator()(glfwGetKey(window, x.first));
    }
    for (auto const& x : mouseBindingMap) {
        x.second->operator()(glfwGetMouseButton(window, x.first), screenMousePos, worldMousePos);
    }
}
void game::KeyboardInputImpl::setKey(int key, KeyboardFunction* func) {
    bindingMap[key] = func;
}
void game::KeyboardInputImpl::replaceKey(int after, int before) {
    KeyboardFunction* temp = bindingMap[before];
    if (bindingMap.count(after) > 0) {
        bindingMap[before] = bindingMap[after];
    } else {
        bindingMap.erase(before);
    }
    bindingMap[after] = bindingMap[before];
}
game::KeyboardInputImpl::~KeyboardInputImpl () {
    bindingMap.clear();
}

void KeyboardInputImpl::setMouseButton (int button, MouseFunction* function) {
    mouseBindingMap[button] = function;
}

void KeyboardInputImpl::onCursorPosChange (event::CursorPosChangeEvent& event) {
    screenMousePos = event.windowPos;
    worldMousePos = event.worldPos;
}

KeyboardInput* game::getKeyboardInput (graphics::Graphics* graphics) {
    return new KeyboardInputImpl(graphics);
}

void game::setupKeyboardInputListeners (KeyboardInput* input, event::EventBus* bus) {
    bus->subscribeHandler(&KeyboardInputImpl::onCursorPosChange, (KeyboardInputImpl*)input);
}