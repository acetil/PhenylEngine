#pragma once

#include "phenyl/plugin.h"
#include "phenyl/runtime.h"

namespace phenyl::core {
class PrefabAssetManager;
}

namespace phenyl {
class PrefabPlugin : public IPlugin {
public:
    PrefabPlugin ();
    ~PrefabPlugin () override;

    [[nodiscard]] std::string_view getName () const noexcept override;
    void init (PhenylRuntime& runtime) override;

private:
    std::unique_ptr<core::PrefabAssetManager> m_manager;
};
} // namespace phenyl
