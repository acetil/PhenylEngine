#include "glcallbacks.h"
#include "graphics/renderers/window_callbacks.h"
#include "graphics/opengl/glrenderer.h"
#include "event/event_bus.h"
#include "common/events/cursor_position_change.h"

using namespace graphics;


static void cursorCallback (GLFWwindow* window, double xPos, double yPos);
static void cursorPosCallback (GLFWwindow* window, double xPos, double yPos);
static void windowSizeCallback (GLFWwindow* window, int width, int height);
static void keyChangeCallback (GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseButtonChangeCallback (GLFWwindow* window, int button, int action, int mods);

void graphics::setupGLWindowCallbacks (GLFWwindow* window) {
    glfwSetCursorPosCallback(window, cursorCallback);
}

void graphics::setupGLWindowCallbacks (GLFWwindow* window, GLWindowCallbackCtx* ctx) {
    glfwSetWindowUserPointer(window, (void*)ctx);

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyChangeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonChangeCallback);
}

void graphics::removeGLWindowCallbacks (GLFWwindow* window) {
    glfwSetCursorPosCallback(window, nullptr);
}


static void cursorCallback (GLFWwindow* window, double xPos, double yPos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    onMousePosChange((WindowCallbackContext*)glfwGetWindowUserPointer(window), xPos, yPos, width, height);
}

static void cursorPosCallback (GLFWwindow* window, double xPos, double yPos) {
    auto* ptr = (GLWindowCallbackCtx*) glfwGetWindowUserPointer(window);

    if (!ptr) {
        return;
    }

    auto windowSize = ptr->renderer->getScreenSize();

    glm::vec2 screenPos = {(float)(xPos / windowSize.x * 2 - 1), -1.0f * (float)(yPos / windowSize.y * 2 - 1)};

    if (!ptr->eventBus.expired()) {
        ptr->eventBus.lock()->raise(event::CursorPosChangeEvent{{xPos, yPos}, screenPos});
    }
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