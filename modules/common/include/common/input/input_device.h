#pragma once

#include <string_view>

#include "input_source.h"

namespace phenyl::common {
    class IInputDevice {
    public:
        virtual ~IInputDevice() = default;

        virtual const ButtonInputSource* getButtonSource (std::string_view sourcePath) = 0;
        virtual const Axis2DInputSource* getAxis2DSource (std::string_view sourcePath) = 0;
        virtual void poll () = 0;

        [[nodiscard]] virtual std::string_view getDeviceId () const noexcept = 0;
    };
}
