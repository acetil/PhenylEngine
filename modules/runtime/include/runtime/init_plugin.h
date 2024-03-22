#pragma once

#include <string_view>

namespace phenyl::runtime {
    class PhenylRuntime;

    class IInitPlugin {
    public:
        virtual ~IInitPlugin() = default;

        [[nodiscard]] virtual std::string_view getName () const noexcept = 0;
        virtual void init (PhenylRuntime& runtime) = 0;
    };
}