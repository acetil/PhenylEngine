#include "glcallbacks.h"
#include "graphics/opengl/glrenderer.h"

using namespace phenyl::graphics;


static void cursorPosCallback (GLFWwindow* window, double xPos, double yPos);
static void windowSizeCallback (GLFWwindow* window, int width, int height);
static void keyChangeCallback (GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseButtonChangeCallback (GLFWwindow* window, int button, int action, int mods);

void phenyl::graphics::setupGLWindowCallbacks (GLFWwindow* window, GLWindowCallbackCtx* ctx) {
    glfwSetWindowUserPointer(window, (void*)ctx);

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyChangeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonChangeCallback);
}

void phenyl::graphics::removeGLWindowCallbacks (GLFWwindow* window) {
    glfwSetCursorPosCallback(window, nullptr);
}

static void cursorPosCallback (GLFWwindow* window, double xPos, double yPos) {
    auto* ptr = (GLWindowCallbackCtx*) glfwGetWindowUserPointer(window);
    if (!ptr) {
        return;
    }

    ptr->renderer->onMousePosChange(glm::vec2{xPos, yPos});
}

static void windowSizeCallback (GLFWwindow* window, int width, int height) {
    auto* ptr = (GLWindowCallbackCtx*) glfwGetWindowUserPointer(window);
    if (!ptr) {
        return;
    }

    ptr->renderer->setScreenSize({width, height});
}

static void keyChangeCallback (GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* ptr = (GLWindowCallbackCtx*) glfwGetWindowUserPointer(window);
    if (!ptr) {
        return;
    }

    //logging::log(LEVEL_DEBUG, "Key change callback! key={}, scancode={}, action={}, mods={}", key, scancode, action, mods);

    ptr->renderer->onKeyChange(key, scancode, action, mods);
}
static void mouseButtonChangeCallback (GLFWwindow* window, int button, int action, int mods) {
    auto* ptr = (GLWindowCallbackCtx*) glfwGetWindowUserPointer(window);
    if (!ptr) {
        return;
    }

    ptr->renderer->onMouseButtonChange(button, action, mods);
}