#pragma once

#include "core/plugin.h"

namespace phenyl::core {
    class Core2DPlugin : public IInitPlugin {
    public:
        Core2DPlugin () = default;
        std::string_view getName() const noexcept override;
        void init (PhenylRuntime &runtime) override;
    };
}