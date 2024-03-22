#include "component/prefab_manager.h"
#include "engine/plugins/prefab_plugin.h"

using namespace phenyl::engine;

PrefabPlugin::PrefabPlugin () = default;
PrefabPlugin::~PrefabPlugin () noexcept = default;

std::string_view PrefabPlugin::getName () const noexcept {
    return "PrefabPlugin";
}

void PrefabPlugin::init (runtime::PhenylRuntime& runtime) {
    manager = std::make_unique<component::PrefabManager>(runtime.manager(), runtime.serializer());
    manager->selfRegister();
}

void PrefabPlugin::shutdown (runtime::PhenylRuntime& runtime) {
    manager->clear();
}