#pragma once

#include "runtime/plugin.h"

namespace phenyl::graphics {
    namespace detail {
        class Graphics;
    }

    class GraphicsPlugin : public runtime::IPlugin {
    private:
        detail::Graphics* graphics;
    public:
        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}