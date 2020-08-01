#include <vector>
#include <utility>

#include "key_input.h"
#include "graphics/graphics_headers.h"
#include "graphics/renderers/glrenderer.h"

using namespace game;
// TODO: add this to renderer
namespace game {
    class KeyboardInputImpl : public KeyboardInput {
    private:
        GLFWwindow* window;
        std::map<int, KeyboardFunction*> bindingMap;
    public:
        KeyboardInputImpl (graphics::Graphics* graphics);

        ~KeyboardInputImpl ();

        void setKey (int key, KeyboardFunction* function);

        void replaceKey (int after, int before);

        void handleKeyPresses ();
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

KeyboardInput* game::getKeyboardInput (graphics::Graphics* graphics) {
    return new KeyboardInputImpl(graphics);
}