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
        std::map<int, std::unique_ptr<KeyboardFunction>> bindingMap;
        std::map<int, std::unique_ptr<MouseFunction>> mouseBindingMap;
        glm::vec2 screenMousePos;
        glm::vec2 worldMousePos;
    public:
        explicit KeyboardInputImpl (graphics::Graphics::SharedPtr graphics);

        ~KeyboardInputImpl ();

        void setKey (int key, std::unique_ptr<KeyboardFunction> function) override;

        void setMouseButton (int button, std::unique_ptr<MouseFunction> function) override;

        void replaceKey (int after, int before) override;

        void handleKeyPresses () override;

        void onCursorPosChange (event::CursorPosChangeEvent& event);
    };
}

game::KeyboardInputImpl::KeyboardInputImpl (graphics::Graphics::SharedPtr graphics) {

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
void game::KeyboardInputImpl::setKey(int key, std::unique_ptr<KeyboardFunction> func) {
    bindingMap[key] = std::move(func);
}
void game::KeyboardInputImpl::replaceKey(int after, int before) {
    auto temp = std::move(bindingMap[before]);
    if (bindingMap.count(after) > 0) {
        bindingMap[before] = std::move(bindingMap[after]);
    } else {
        bindingMap.erase(before);
    }
    bindingMap[after] = std::move(temp);
}
game::KeyboardInputImpl::~KeyboardInputImpl () {
    bindingMap.clear();
}

void KeyboardInputImpl::setMouseButton (int button, std::unique_ptr<MouseFunction> function) {
    mouseBindingMap[button] = std::move(function);
}

void KeyboardInputImpl::onCursorPosChange (event::CursorPosChangeEvent& event) {
    screenMousePos = event.windowPos;
    worldMousePos = event.worldPos;
}

KeyboardInput::SharedPtr game::getKeyboardInput (const graphics::Graphics::SharedPtr& graphics) {
    return std::make_shared<KeyboardInputImpl>(graphics);
}

void game::setupKeyboardInputListeners (const KeyboardInput::SharedPtr& input, const event::EventBus::SharedPtr& bus) {
    bus->subscribeHandler(&KeyboardInputImpl::onCursorPosChange, std::dynamic_pointer_cast<KeyboardInputImpl>(input));
}