#pragma once

#include "runtime/plugin.h"

namespace phenyl::common {
    class InputPlugin : public runtime::IPlugin {
    public:
        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void frameBegin (runtime::PhenylRuntime& runtime) override;
    };
}
