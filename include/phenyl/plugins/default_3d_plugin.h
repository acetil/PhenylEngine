#pragma once

#include "phenyl/plugin.h"

namespace phenyl {
    class Default3DPlugin : public IInitPlugin {
    public:
        std::string_view getName () const noexcept override;
        void init (core::PhenylRuntime& runtime) override;
    };
}
