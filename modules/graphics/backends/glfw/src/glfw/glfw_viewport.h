#pragma once

#include "glfw_headers.h"
#include "graphics/graphics_properties.h"
#include "graphics/viewport.h"
#include "input/glfw_key_input.h"
#include "input/glfw_mouse_input.h"

#include <functional>

namespace phenyl::glfw {
class GLFWViewport : public graphics::Viewport {
public:
    explicit GLFWViewport (
        const graphics::GraphicsProperties& properties, const std::function<void()>& windowHintCallback,
        const std::function<void(GLFWwindow*)>& postInitCallback = [] (auto*) {});
    ~GLFWViewport () override;

    explicit operator bool () const;

    [[nodiscard]] bool shouldClose () const override;
    void poll () override;
    [[nodiscard]] glm::ivec2 getResolution () const override;
    glm::vec2 getContentScale () const override;

    void addUpdateHandler (graphics::IViewportUpdateHandler* handler) override;
    void addInputDevices (core::GameInput& manager) override;

    [[nodiscard]] double getTime () const;
    void swapBuffers ();
    void setupCallbacks ();

protected:
    GLFWwindow* m_window = nullptr;

private:
    glm::ivec2 m_resolution;
    glm::vec2 m_cursorPos{0, 0};

    std::unique_ptr<GLFWKeyInput> m_keyInput;
    std::unique_ptr<GLFWMouseInput> m_mouseInput;

    std::vector<graphics::IViewportUpdateHandler*> m_updateHandlers;

    void onCursorPosCallback (glm::vec2 pos);
    void onWindowSizeCallback (glm::ivec2 newRes);
    void onKeyChange (int scancode, int action, int mods);
    void onButtonChange (int button, int action, int mods);

    std::string_view getName () const noexcept override;
};
} // namespace phenyl::glfw
