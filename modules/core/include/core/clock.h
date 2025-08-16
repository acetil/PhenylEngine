#pragma once
#include "iresource.h"

namespace phenyl::core {
class Clock : public IResource {
public:
    std::string_view getName () const noexcept override {
        return "phenyl::Clock";
    }

    [[nodiscard]] virtual double deltaTime () const noexcept = 0;

    [[nodiscard]] virtual double variableDeltaTime () const noexcept = 0;
    [[nodiscard]] virtual double fixedDeltaTime () const noexcept = 0;
};
} // namespace phenyl::core
