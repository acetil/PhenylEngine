#include "glcallbacks.h"
#include "window_callbacks.h"

using namespace graphics;


static void cursorCallback (GLFWwindow* window, double xPos, double yPos);

void graphics::setupGLWindowCallbacks (GLFWwindow* window) {
    glfwSetCursorPosCallback(window, cursorCallback);
}


static void cursorCallback (GLFWwindow* window, double xPos, double yPos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    onMousePosChange((WindowCallbackContext*)glfwGetWindowUserPointer(window), xPos, yPos, width, height);
}