#pragma once

#include <string_view>

namespace phenyl::runtime {
    class IResource {
    public:
        virtual ~IResource() = default;

        [[nodiscard]] virtual std::string_view getName () const noexcept = 0;
    };
}