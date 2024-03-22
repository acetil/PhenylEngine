#pragma once

#include "runtime/init_plugin.h"

namespace phenyl::common {
    class Core2DPlugin : public runtime::IInitPlugin {
    public:
        Core2DPlugin () = default;
        std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime &runtime) override;
    };
}