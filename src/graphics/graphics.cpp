#include "graphics.h"
using namespace graphics;

graphics::Graphics::Graphics (GLFWwindow* window) {
    this->window = window;
    shaderMap = std::unordered_map<std::string, graphics::ShaderProgram>();
}