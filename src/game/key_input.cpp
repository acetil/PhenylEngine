#include <vector>
#include <utility>

#include "key_input.h"
#include "graphics/graphics_headers.h"

using namespace game;

game::KeyboardInput::KeyboardInput (GLFWwindow* window) {
    this->window = window;
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    // TODO: add key press preference reading here
}

void game::KeyboardInput::handleKeyPresses () {
    for (auto const& x : bindingMap) {
        x.second->operator()(glfwGetKey(window, x.first));
    }
}
void game::KeyboardInput::setKey(int key, KeyboardFunction* func) {
    bindingMap[key] = func;
}
void game::KeyboardInput::replaceKey(int after, int before) {
    KeyboardFunction* temp = bindingMap[before];
    if (bindingMap.count(after) > 0) {
        bindingMap[before] = bindingMap[after];
    } else {
        bindingMap.erase(before);
    }
    bindingMap[after] = bindingMap[before];
}
game::KeyboardInput::~KeyboardInput () {
    bindingMap.clear();
}