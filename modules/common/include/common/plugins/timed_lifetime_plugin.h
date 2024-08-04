#pragma once

#include "runtime/plugin.h"

namespace phenyl::common {
    class TimedLifetimePlugin : public runtime::IInitPlugin {
    public:
        std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime &runtime) override;
    };
}