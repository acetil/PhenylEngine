#pragma once

#include "util/map.h"

#include "core/input/input_device.h"

namespace phenyl::graphics {
    class GLFWKeyInput : public core::IInputDevice {
    private:
        GLFWwindow* window;
        util::HashMap<std::string, int> buttonIds;
        util::HashMap<int, core::ButtonInputSource> sources;
    public:
        GLFWKeyInput (GLFWwindow* window);

        const core::ButtonInputSource* getButtonSource (std::string_view sourcePath) override;
        const core::Axis2DInputSource* getAxis2DSource (std::string_view sourcePath) override;
        std::string_view getDeviceId () const noexcept override;
        void poll () override;
    };
}