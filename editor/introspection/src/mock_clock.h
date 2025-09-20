#pragma once
#include "phenyl/runtime.h"

namespace phenyl {
class MockClock : public Clock {
public:
    [[nodiscard]] double deltaTime () const noexcept override {
        return 0.1;
    }

    [[nodiscard]] double variableDeltaTime () const noexcept override {
        return 0.1;
    }

    [[nodiscard]] double fixedDeltaTime () const noexcept override {
        return 0.1;
    }
};
} // namespace phenyl
