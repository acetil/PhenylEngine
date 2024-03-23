#pragma once

#include "runtime/init_plugin.h"

namespace phenyl::engine {
    class Default2DPlugin : public runtime::IInitPlugin {
    public:
        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}