#pragma once

#include "phenyl/plugin.h"
#include "phenyl/runtime.h"

namespace phenyl {
    class LevelPlugin : public IInitPlugin {
    public:
        LevelPlugin ();
        ~LevelPlugin() override;

        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (PhenylRuntime& runtime) override;
    };
}