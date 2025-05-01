#pragma once

#include "core/input/input_device.h"
#include "glfw/glfw_headers.h"
#include "util/map.h"

// TODO
namespace phenyl::graphics::detail {
    extern phenyl::Logger GRAPHICS_LOGGER;
}

namespace phenyl::glfw {
    class GLFWMouseInput : public core::IInputDevice {
    public:
        GLFWMouseInput (GLFWwindow* window);

        const core::ButtonInputSource* getButtonSource (std::string_view sourcePath) override;
        const core::Axis2DInputSource* getAxis2DSource (std::string_view sourcePath) override;
        std::string_view getDeviceId () const noexcept override;
        void poll () override;

    private:
        GLFWwindow* m_window;
        util::HashMap<std::string, int> m_buttonIds;
        util::HashMap<int, core::ButtonInputSource> m_sources;
        core::Axis2DInputSource m_mousePosSource;
    };
}