#pragma once

#include "runtime/plugin.h"

namespace phenyl::engine {
    class LevelPlugin : public runtime::IInitPlugin {
    public:
        LevelPlugin ();
        ~LevelPlugin() override;

        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}