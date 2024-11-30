#pragma once

#include "core/plugin.h"

namespace phenyl::core {
    class InputPlugin : public IInitPlugin {
    public:
        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (PhenylRuntime& runtime) override;
    };
}
