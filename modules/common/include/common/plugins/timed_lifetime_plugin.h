#pragma once

#include "runtime/plugin.h"

namespace phenyl::common {
    class TimedLifetimePlugin : public runtime::IPlugin {
    public:
        std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime &runtime) override;
        void update(runtime::PhenylRuntime &runtime, double deltaTime) override;
    };
}