#include "core/input/game_input.h"
#include "graphics/detail/loggers.h"

#include "glfw_viewport.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GLFW_VIEWPORT", detail::GRAPHICS_LOGGER};

GLFWViewport::GLFWViewport (const GraphicsProperties& properties) {
    glewExperimental = true;

    if (!glfwInit()) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            PHENYL_LOGE(LOGGER, "glfwInit() error code {}: {}", code, glfwError);
        }
        return;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(properties.getWindowWidth(), properties.getWindowHeight(), properties.getWindowTitle().c_str(), nullptr, nullptr);
    if (!window) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            PHENYL_LOGE(LOGGER, "glfwCreatWindow() error code {}: {}", code, glfwError);
        }

        PHENYL_ABORT("Failed to open GLFW window! The GPU may not be compatible with OpenGL 3.3!");
    }

    glfwMakeContextCurrent(window);

    // TODO: move out
    glewExperimental = true; // TODO: check if removal affects anything
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        window = nullptr;
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(properties.getVsync() ? 1 : 0); // TODO: handle enable/disable vsync
    resolution = {properties.getWindowWidth(), properties.getWindowHeight()};

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    keyInput = std::make_unique<GLFWKeyInput>(window);
    mouseInput = std::make_unique<GLFWMouseInput>(window);

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    cursorPos = glm::vec2{cursorX, cursorY};

    setupCallbacks();
}

GLFWViewport::~GLFWViewport () {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}


GLFWViewport::operator bool () const {
    return window;
}

bool GLFWViewport::shouldClose () const {
    return glfwWindowShouldClose(window);
}

void GLFWViewport::poll () {
    glfwPollEvents();
}

double GLFWViewport::getTime () const {
    return glfwGetTime();
}

void GLFWViewport::swapBuffers () {
    glfwSwapBuffers(window);
}

glm::ivec2 GLFWViewport::getResolution () const {
    return resolution;
}

glm::vec2 GLFWViewport::getContentScale () const {
    glm::vec2 scale;
    glfwGetWindowContentScale(window, &scale.x, &scale.y);
    return scale;
}

void GLFWViewport::onCursorPosCallback (glm::vec2 pos) {
    cursorPos = pos;
}

void GLFWViewport::onWindowSizeCallback (glm::ivec2 newRes) {
    auto oldRes = resolution;
    resolution = newRes;

    for (auto* handler : updateHandlers) {
        handler->onViewportResize(oldRes, resolution);
    }
}

void GLFWViewport::onKeyChange (int scancode, int action, int mods) {
    //keyInput->onButtonChange(scancode, action, mods);
}

void GLFWViewport::onButtonChange (int button, int action, int mods) {
    //mouseInput->onButtonChange(button, action, mods);
}

std::string_view GLFWViewport::getName () const noexcept {
    return "GLFWViewport";
}

void GLFWViewport::setupCallbacks () {
    glfwSetWindowUserPointer(window, (void*)this);

    glfwSetCursorPosCallback(window, [] (GLFWwindow* window, double xPos, double yPos) {
        PHENYL_DASSERT(window);
         auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onCursorPosCallback({xPos, yPos});
    });

    glfwSetWindowSizeCallback(window, [] (GLFWwindow* window, int width, int height) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onWindowSizeCallback({width, height});
    });

    glfwSetKeyCallback(window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onKeyChange(scancode, action, mods);
    });

    glfwSetMouseButtonCallback(window, [] (GLFWwindow* window, int button, int action, int mods) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onButtonChange(button, action, mods);
    });
}

void GLFWViewport::addUpdateHandler (IViewportUpdateHandler* handler) {
    PHENYL_DASSERT(handler);
    updateHandlers.emplace_back(handler);
}

void GLFWViewport::addInputDevices (core::GameInput& manager) {
    manager.addDevice(keyInput.get());
    manager.addDevice(mouseInput.get());
}
