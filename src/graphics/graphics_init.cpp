#include <stdio.h>

#include "graphics_headers.h"
#include "graphics.h"
#include "graphics_init.h"
#include "logging/logging.h"

using namespace graphics;

int graphics::initWindow (GLFWwindow** windowPtr) {
    glewExperimental = true;
    if (!glfwInit()) {
        logging::log(LEVEL_FATAL, "Failed to init GLFW!");
        return GRAPHICS_INIT_FAILURE;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, DEFAULT_WINDOW_NAME, NULL, NULL);
    if (window == NULL) {
        logging::log(LEVEL_FATAL, "Failed to open GLFW window! Your GPU may not be compatible with OpenGl 3.3!");
        return GRAPHICS_INIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true; // TODO: check if removal affects anything
    if (glewInit() != GLEW_OK) {
        logging::log(LEVEL_FATAL, "Failed to initialse GLEW!");
        return GRAPHICS_INIT_FAILURE;
    }
    *windowPtr = window;
    logging::log(LEVEL_INFO, "Window initialised successfully!");
    return GRAPHICS_INIT_SUCCESS;
}
int graphics::initGraphics (GLFWwindow* window, Graphics* graphicsPtr) {
    Graphics graphics(window);
    return GRAPHICS_INIT_SUCCESS;
}