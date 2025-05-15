#pragma once

#include "iresource.h"

namespace phenyl::core {
struct DeltaTime : public IResource {
public:
    DeltaTime () = default;

    [[nodiscard]] double operator() () const noexcept {
        return m_value;
    }

    void set (const double value) noexcept {
        this->m_value = value;
    }

    [[nodiscard]] std::string_view getName () const noexcept override {
        return "DeltaTime";
    }

private:
    double m_value = 0.0;
};

struct FixedDelta : public IResource {
public:
    FixedDelta () = default;

    [[nodiscard]] double operator() () const noexcept {
        return m_value;
    }

    void set (const double value) noexcept {
        this->m_value = value;
    }

    std::string_view getName () const noexcept override {
        return "FixedDelta";
    }

private:
    double m_value = 0.0;
};
} // namespace phenyl::core
