#pragma once

#include "glfw_headers.h"

#include "graphics/viewport.h"
#include "graphics/graphics_properties.h"
#include "input/glfw_key_input.h"
#include "input/glfw_mouse_input.h"

namespace phenyl::graphics {
    class GLFWViewport : public Viewport {
    public:
        explicit GLFWViewport (const GraphicsProperties& properties);
        ~GLFWViewport() override;

        explicit operator bool () const;

        [[nodiscard]] bool shouldClose () const override;
        void poll () override;
        [[nodiscard]] glm::ivec2 getResolution() const override;
        glm::vec2 getContentScale() const override;

        void addUpdateHandler (IViewportUpdateHandler* handler) override;
        void addInputDevices (core::GameInput& manager) override;

        [[nodiscard]] double getTime () const;
        void swapBuffers ();
        void setupCallbacks ();
    private:
        GLFWwindow* window = nullptr;
        glm::ivec2 resolution;
        glm::vec2 cursorPos{0, 0};

        std::unique_ptr<GLFWKeyInput> keyInput;
        std::unique_ptr<GLFWMouseInput> mouseInput;

        std::vector<IViewportUpdateHandler*> updateHandlers;

        void onCursorPosCallback (glm::vec2 pos);
        void onWindowSizeCallback (glm::ivec2 newRes);
        void onKeyChange (int scancode, int action, int mods);
        void onButtonChange (int button, int action, int mods);

        std::string_view getName() const noexcept override;
    };
}
