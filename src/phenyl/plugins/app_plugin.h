#pragma once

#include <memory>

#include "core/plugin.h"

namespace phenyl::engine {
    class ApplicationBase;

    class AppPlugin : public core::IPlugin {
    private:
        std::unique_ptr<ApplicationBase> app;
    public:
        explicit AppPlugin (std::unique_ptr<ApplicationBase> app);

        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (core::PhenylRuntime &runtime) override;
        //void update (runtime::PhenylRuntime &runtime, double deltaTime) override;
        //void fixedUpdate (runtime::PhenylRuntime &runtime, double deltaTime) override;
        void shutdown (core::PhenylRuntime &runtime) override;
    };
}