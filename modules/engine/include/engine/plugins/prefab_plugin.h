#pragma once

#include "runtime/plugin.h"

namespace phenyl::component {
    class PrefabManager;
}

namespace phenyl::engine {
    class PrefabPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<component::PrefabManager> manager;
    public:
        PrefabPlugin ();
        ~PrefabPlugin();

        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
        void shutdown(runtime::PhenylRuntime& runtime) override;
    };
}