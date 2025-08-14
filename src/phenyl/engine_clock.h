#pragma once

#include "core/clock.h"

#include <chrono>

namespace phenyl::engine {
class EngineClock : public core::Clock {
public:
    using Duration = std::chrono::nanoseconds;

    explicit EngineClock (Duration fixedDeltaTime);

    void advance (double fixedTimeScale);

    bool startFixedFrame ();
    void startVariableFrame ();

    Duration frameTime () const noexcept;

    [[nodiscard]] double deltaTime () const noexcept override;
    [[nodiscard]] double fixedDeltaTime () const noexcept override;
    [[nodiscard]] double variableDeltaTime () const noexcept override;

private:
    using ClockType = std::chrono::steady_clock;

    Duration m_fixedDeltaTime;

    ClockType::time_point m_prevFrameTime;
    Duration m_deltaTime{0};
    Duration m_fixedTimeSlop{0};
    bool isFixed = false;
};
} // namespace phenyl::engine
