#pragma once

#include "input_source.h"

#include <string_view>

namespace phenyl::core {
class IInputDevice {
public:
    virtual ~IInputDevice () = default;

    virtual const ButtonInputSource* getButtonSource (std::string_view sourcePath) = 0;
    virtual const Axis2DInputSource* getAxis2DSource (std::string_view sourcePath) = 0;
    virtual void poll () = 0;

    [[nodiscard]] virtual std::string_view getDeviceId () const noexcept = 0;
};
} // namespace phenyl::core
