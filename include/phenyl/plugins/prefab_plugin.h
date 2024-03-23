#pragma once

#include "phenyl/plugin.h"
#include "phenyl/runtime.h"

namespace phenyl::component {
    class PrefabManager;
}

namespace phenyl {
    class PrefabPlugin : public IPlugin {
    private:
        std::unique_ptr<component::PrefabManager> manager;
    public:
        PrefabPlugin ();
        ~PrefabPlugin();

        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (PhenylRuntime& runtime) override;
        void shutdown(PhenylRuntime& runtime) override;
    };
}