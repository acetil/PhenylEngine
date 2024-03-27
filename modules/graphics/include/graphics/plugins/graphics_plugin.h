#pragma once

#include "runtime/plugin.h"

namespace phenyl::graphics {
    class DebugLayer;

    class GraphicsPlugin : public runtime::IPlugin {
    private:
        DebugLayer* debugLayer;
    public:
        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void render (phenyl::runtime::PhenylRuntime &runtime) override;
    };
}