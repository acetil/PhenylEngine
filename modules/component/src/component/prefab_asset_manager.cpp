#include <iostream>


#include "component/prefab_asset_manager.h"
#include "component/prefab.h"
#include "component/component_serializer.h"
#include "component/detail/loggers.h"

#include "common/assets/assets.h"

using namespace phenyl;

static Logger LOGGER{"PREFAB_ASSET_MANAGER", component::detail::COMPONENT_LOGGER};

component::Prefab* component::PrefabAssetManager::load (std::istream& data, std::size_t id) {
    PHENYL_DASSERT(!prefabs.contains(id));
    nlohmann::json json;
    data >> json;

    if (!json.is_object()) {
        PHENYL_LOGE(LOGGER, "Expected object for prefab, got {}!", json.type_name());
        return nullptr;
    }

    if (!json.contains("components")) {
        PHENYL_LOGE(LOGGER, "Failed to find components member of prefab!");
        return nullptr;
    }

    auto builder = world.buildPrefab();
    serializer.deserializePrefab(builder, json.at("components"));

    auto prefab = std::make_unique<Prefab>(builder.build());
    auto* ptr = prefab.get();
    prefabs[id] = std::move(prefab);

    PHENYL_LOGD(LOGGER, "Loaded prefab {}!", id);

    return ptr;
}

void component::PrefabAssetManager::queueUnload (std::size_t id) {
    PHENYL_LOGD(LOGGER, "Unload requested for prefab {}!", id);
}

void component::PrefabAssetManager::selfRegister () {
    common::Assets::AddManager(this);
}

component::PrefabAssetManager::~PrefabAssetManager () {
    common::Assets::RemoveManager(this);
}

const char* component::PrefabAssetManager::getFileType () const {
    return ".json";
}

void component::PrefabAssetManager::clear () {
    prefabs.clear();
}

component::Prefab* component::PrefabAssetManager::load (component::Prefab&& obj, std::size_t id) {
    prefabs[id] = std::make_unique<component::Prefab>(std::move(obj));
    return prefabs[id].get();
}
