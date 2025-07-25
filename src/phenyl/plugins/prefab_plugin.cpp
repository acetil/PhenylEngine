#include "phenyl/plugins/prefab_plugin.h"

#include "core/component/prefab_asset_manager.h"

using namespace phenyl;

PrefabPlugin::PrefabPlugin () = default;
PrefabPlugin::~PrefabPlugin () = default;

std::string_view PrefabPlugin::getName () const noexcept {
    return "PrefabPlugin";
}

void PrefabPlugin::init (PhenylRuntime& runtime) {
    m_manager = std::make_unique<core::PrefabAssetManager>(runtime.world(), runtime.serializer());
    m_manager->selfRegister();
}
