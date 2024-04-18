#pragma once

#include "common/input/input_device.h"
#include "graphics/graphics_headers.h"
#include "util/map.h"

namespace phenyl::graphics {
    class GLFWMouseInput : public common::IInputDevice {
    private:
        GLFWwindow* window;
        util::HashMap<std::string, int> buttonIds;
        util::HashMap<int, common::ButtonInputSource> sources;
    public:
        GLFWMouseInput (GLFWwindow* window);

        const common::ButtonInputSource* getButtonSource (std::string_view sourcePath) override;
        std::string_view getDeviceId () const noexcept override;
        void poll () override;
    };
}