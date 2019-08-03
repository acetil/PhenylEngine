#include "graphics.h"
#include "graphics_headers.h"
using namespace graphics;

graphics::Graphics::Graphics (GLFWwindow* window) {
    this->window = window;
    shaderMap = std::unordered_map<std::string, graphics::ShaderProgram>();
}

bool graphics::Graphics::shouldClose () {
    return window == NULL || glfwWindowShouldClose(window) != 0;
}
void graphics::Graphics::render () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: add render code
    
    glfwSwapBuffers(window);
}
void graphics::Graphics::pollEvents () {
    glfwPollEvents();
}