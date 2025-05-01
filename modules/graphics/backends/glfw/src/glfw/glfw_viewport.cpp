#include "core/input/game_input.h"
#include "logging/logging.h"
#include "util/profiler.h"

#include "glfw_viewport.h"


using namespace phenyl::glfw;

phenyl::Logger detail::GLFW_LOGGER{"GLFW", phenyl::PHENYL_LOGGER};

GLFWViewport::GLFWViewport (const graphics::GraphicsProperties& properties, const std::function<void()>& windowHintCallback, const std::function<void(GLFWwindow*)>& postInitCallback) {
    //glewExperimental = true;

    if (!glfwInit()) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            PHENYL_LOGE(detail::GLFW_LOGGER, "glfwInit() error code {}: {}", code, glfwError);
        }
        return;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    windowHintCallback();

    m_window = glfwCreateWindow(properties.getWindowWidth(), properties.getWindowHeight(), properties.getWindowTitle().c_str(), nullptr, nullptr);
    if (!m_window) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError)) != GLFW_NO_ERROR) {
            PHENYL_LOGE(detail::GLFW_LOGGER, "glfwCreateWindow() error code {}: {}", code, glfwError);
        }

        PHENYL_ABORT("Failed to open GLFW window!");
    }

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(properties.getVsync() ? 1 : 0); // TODO: handle enable/disable vsync
    m_resolution = {properties.getWindowWidth(), properties.getWindowHeight()};

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    m_keyInput = std::make_unique<GLFWKeyInput>(m_window);
    m_mouseInput = std::make_unique<GLFWMouseInput>(m_window);

    double cursorX, cursorY;
    glfwGetCursorPos(m_window, &cursorX, &cursorY);
    m_cursorPos = glm::vec2{cursorX, cursorY};

    postInitCallback(m_window);
    PHENYL_LOGI(detail::GLFW_LOGGER, "Initialised GLFW viewport");
    setupCallbacks();

    util::setProfilerTimingFunction(glfwGetTime);
}

GLFWViewport::~GLFWViewport () {
    if (m_window) {
        PHENYL_LOGI(detail::GLFW_LOGGER, "Destroying GLFW viewport");
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}


GLFWViewport::operator bool () const {
    return m_window;
}

bool GLFWViewport::shouldClose () const {
    return glfwWindowShouldClose(m_window);
}

void GLFWViewport::poll () {
    glfwPollEvents();
}

double GLFWViewport::getTime () const {
    return glfwGetTime();
}

void GLFWViewport::swapBuffers () {
    glfwSwapBuffers(m_window);
}

glm::ivec2 GLFWViewport::getResolution () const {
    return m_resolution;
}

glm::vec2 GLFWViewport::getContentScale () const {
    glm::vec2 scale;
    glfwGetWindowContentScale(m_window, &scale.x, &scale.y);
    return scale;
}

void GLFWViewport::onCursorPosCallback (glm::vec2 pos) {
    m_cursorPos = pos;
}

void GLFWViewport::onWindowSizeCallback (glm::ivec2 newRes) {
    auto oldRes = m_resolution;
    m_resolution = newRes;

    for (auto* handler : m_updateHandlers) {
        handler->onViewportResize(oldRes, m_resolution);
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
    glfwSetWindowUserPointer(m_window, (void*)this);

    glfwSetCursorPosCallback(m_window, [] (GLFWwindow* window, double xPos, double yPos) {
        PHENYL_DASSERT(window);
         auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onCursorPosCallback({xPos, yPos});
    });

    glfwSetWindowSizeCallback(m_window, [] (GLFWwindow* window, int width, int height) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onWindowSizeCallback({width, height});
    });

    glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onKeyChange(scancode, action, mods);
    });

    glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, int button, int action, int mods) {
        PHENYL_DASSERT(window);
        auto* ptr = static_cast<GLFWViewport*>(glfwGetWindowUserPointer(window));
        PHENYL_DASSERT(ptr);

        ptr->onButtonChange(button, action, mods);
    });
}

void GLFWViewport::addUpdateHandler (graphics::IViewportUpdateHandler* handler) {
    PHENYL_DASSERT(handler);
    m_updateHandlers.emplace_back(handler);
}

void GLFWViewport::addInputDevices (core::GameInput& manager) {
    manager.addDevice(m_keyInput.get());
    manager.addDevice(m_mouseInput.get());
}
