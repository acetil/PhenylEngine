#pragma once

#include "graphics/graphics_headers.h"

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
        [[nodiscard]] glm::vec2 getCursorPos() const override; // TODO

        [[nodiscard]] std::vector<std::shared_ptr<phenyl::common::InputSource>> getInputSources () const override;
        [[nodiscard]] std::vector<std::shared_ptr<phenyl::common::ProxySource>> getProxySources () const override;

        [[nodiscard]] double getTime () const;
        void swapBuffers ();
        void setupCallbacks ();
    private:
        GLFWwindow* window = nullptr;
        glm::ivec2 resolution;
        glm::vec2 cursorPos{0, 0};

        std::shared_ptr<GLFWKeyInput2> keyInput;
        std::shared_ptr<GLFWMouseInput2> mouseInput;
        std::vector<std::shared_ptr<common::ProxySource>> proxySources;

        void onCursorPosCallback (glm::vec2 pos);
        void onWindowSizeCallback (glm::ivec2 newRes);
        void onKeyChange (int scancode, int action, int mods);
        void onButtonChange (int button, int action, int mods);
    };
}