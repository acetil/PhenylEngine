#pragma once

#include "iresource.h"

namespace phenyl::runtime {
    struct DeltaTime : public IResource {
    private:
        double value = 0.0;
    public:
        DeltaTime () = default;

        [[nodiscard]] double operator() () const noexcept {
            return value;
        }

        void set (const double value) noexcept {
            this->value = value;
        }

        [[nodiscard]] std::string_view getName () const noexcept override {
            return "DeltaTime";
        }
    };

    struct FixedDelta : public IResource {
    private:
        double value = 0.0;
    public:
        FixedDelta () = default;

        [[nodiscard]] double operator() () const noexcept {
            return value;
        }

        void set (const double value) noexcept {
            this->value = value;
        }

        std::string_view getName () const noexcept override {
            return "FixedDelta";
        }
    };
}