#include <iostream>

#include "component/prefab_manager.h"
#include "component/prefab.h"
#include "component/component_serializer.h"

#include "common/assets/assets.h"

using namespace phenyl;

component::Prefab* component::PrefabManager::load (std::istream& data, std::size_t id) {
    PHENYL_DASSERT(!prefabs.contains(id));
    nlohmann::json json;
    data >> json;

    if (!json.is_object()) {
        PHENYL_LOGE(detail::PREFAB_LOGGER, "Expected object for prefab, got {}!", json.type_name());
        return nullptr;
    }

    if (!json.contains("components")) {
        PHENYL_LOGE(detail::PREFAB_LOGGER, "Failed to find components member of prefab!");
        return nullptr;
    }

    auto builder = manager->buildPrefab();
    serializer->deserializePrefab(builder, json.at("components"));

    auto prefab = std::make_unique<Prefab>(builder.build());
    auto* ptr = prefab.get();
    prefabs[id] = std::move(prefab);

    PHENYL_LOGD(detail::PREFAB_LOGGER, "Loaded prefab {}!", id);

    return ptr;
}

void component::PrefabManager::queueUnload (std::size_t id) {
    PHENYL_LOGD(detail::PREFAB_LOGGER, "Unload requested for prefab {}!", id);
}

void component::PrefabManager::selfRegister () {
    common::Assets::AddManager(this);
}

component::PrefabManager::~PrefabManager () {
    common::Assets::RemoveManager(this);
}

const char* component::PrefabManager::getFileType () const {
    return ".json";
}

void component::PrefabManager::clear () {
    prefabs.clear();
}

component::Prefab* component::PrefabManager::load (component::Prefab&& obj, std::size_t id) {
    prefabs[id] = std::make_unique<component::Prefab>(std::move(obj));
    return prefabs[id].get();
}
