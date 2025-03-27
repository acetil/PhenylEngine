#pragma once

#include "core/input/input_device.h"
#include "glfw/glfw_headers.h"
#include "util/map.h"

namespace phenyl::graphics {
    // TODO
    namespace detail {
        extern phenyl::Logger GRAPHICS_LOGGER;
    }

    class GLFWMouseInput : public core::IInputDevice {
    private:
        GLFWwindow* window;
        util::HashMap<std::string, int> buttonIds;
        util::HashMap<int, core::ButtonInputSource> sources;
        core::Axis2DInputSource mousePosSource;
    public:
        GLFWMouseInput (GLFWwindow* window);

        const core::ButtonInputSource* getButtonSource (std::string_view sourcePath) override;
        const core::Axis2DInputSource* getAxis2DSource (std::string_view sourcePath) override;
        std::string_view getDeviceId () const noexcept override;
        void poll () override;
    };
}