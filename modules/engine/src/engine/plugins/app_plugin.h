#pragma once

#include "engine/application.h"
#include "runtime/plugin.h"

namespace phenyl::engine {
    class AppPlugin : public runtime::IPlugin {
    private:
        Application* app;
    public:
        explicit AppPlugin (Application* app);

        [[nodiscard]] std::string_view getName() const noexcept override;
        void init (runtime::PhenylRuntime &runtime) override;
        void update (runtime::PhenylRuntime &runtime, double deltaTime) override;
        void fixedUpdate (runtime::PhenylRuntime &runtime, double deltaTime) override;
        void shutdown (runtime::PhenylRuntime &runtime) override;
    };
}