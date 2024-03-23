#pragma once

#include "phenyl/plugin.h"
#include "phenyl/runtime.h"

namespace phenyl {
    class Default2DPlugin : public IInitPlugin {
    public:
        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (PhenylRuntime& runtime) override;
    };
}