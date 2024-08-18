#pragma once

#include "phenyl/plugin.h"
#include "phenyl/runtime.h"

namespace phenyl::component {
    class PrefabAssetManager;
}

namespace phenyl {
    class PrefabPlugin : public IPlugin {
    private:
        std::unique_ptr<component::PrefabAssetManager> manager;
    public:
        PrefabPlugin ();
        ~PrefabPlugin() override;

        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (PhenylRuntime& runtime) override;
        void shutdown(PhenylRuntime& runtime) override;
    };
}