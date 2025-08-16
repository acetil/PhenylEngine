#include "engine_clock.h"

using namespace phenyl::engine;

EngineClock::EngineClock (Duration fixedDeltaTime) :
    m_fixedDeltaTime{fixedDeltaTime},
    m_prevFrameTime{ClockType::now()} {}

void EngineClock::advance (double fixedTimeScale) {
    auto now = ClockType::now();
    m_deltaTime = now - m_prevFrameTime;
    m_fixedTimeSlop += std::chrono::floor<Duration>((now - m_prevFrameTime) * fixedTimeScale);
    m_prevFrameTime = now;
}

bool EngineClock::startFixedFrame () {
    if (m_fixedTimeSlop >= m_fixedDeltaTime) {
        m_fixedTimeSlop -= m_fixedDeltaTime;
        isFixed = true;
        return true;
    }
    return false;
}

void EngineClock::startVariableFrame () {
    isFixed = false;
}

EngineClock::Duration EngineClock::frameTime () const noexcept {
    return ClockType::now() - m_prevFrameTime;
}

double EngineClock::deltaTime () const noexcept {
    return isFixed ? fixedDeltaTime() : variableDeltaTime();
}

double EngineClock::fixedDeltaTime () const noexcept {
    return std::chrono::duration_cast<std::chrono::duration<double>>(m_fixedDeltaTime).count();
}

double EngineClock::variableDeltaTime () const noexcept {
    return std::chrono::duration_cast<std::chrono::duration<double>>(m_deltaTime).count();
}
