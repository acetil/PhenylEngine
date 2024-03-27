#pragma once

#include "runtime/plugin.h"

namespace phenyl::graphics {
    namespace detail {
        class Graphics;
    }

    class DebugLayer;

    class GraphicsPlugin : public runtime::IPlugin {
    private:
        detail::Graphics* graphics;
        DebugLayer* debugLayer;
    public:
        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void render (phenyl::runtime::PhenylRuntime &runtime) override;
    };
}