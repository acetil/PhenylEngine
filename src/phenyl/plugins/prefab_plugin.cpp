#include "component/prefab_asset_manager.h"
#include "phenyl/plugins/prefab_plugin.h"

using namespace phenyl;

PrefabPlugin::PrefabPlugin () = default;
PrefabPlugin::~PrefabPlugin () = default;

std::string_view PrefabPlugin::getName () const noexcept {
    return "PrefabPlugin";
}

void PrefabPlugin::init (PhenylRuntime& runtime) {
    manager = std::make_unique<component::PrefabAssetManager>(runtime.world(), runtime.serializer());
    manager->selfRegister();
}

void PrefabPlugin::shutdown (PhenylRuntime& runtime) {
    manager->clear();
}