#pragma once

#include "util/map.h"

#include "common/input/input_device.h"

namespace phenyl::graphics {
    class GLFWKeyInput : public common::IInputDevice {
    private:
        GLFWwindow* window;
        util::HashMap<std::string, int> buttonIds;
        util::HashMap<int, common::ButtonInputSource> sources;
    public:
        GLFWKeyInput (GLFWwindow* window);

        const common::ButtonInputSource* getButtonSource (std::string_view sourcePath) override;
        std::string_view getDeviceId () const noexcept override;
        void poll () override;
    };
}