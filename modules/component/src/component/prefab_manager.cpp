#include <iostream>

#include "component/prefab_manager.h"
#include "component/prefab.h"
#include "component/component_serializer.h"

#include "common/assets/assets.h"

using namespace phenyl;

component::Prefab* component::PrefabManager::load (std::istream& data, std::size_t id) {
    assert(!prefabs.contains(id));
    nlohmann::json json;
    data >> json;

    if (!json.is_object()) {
        logging::log(LEVEL_ERROR, "Expected object for prefab, got {}!", json.type_name());
        return nullptr;
    }

    if (!json.contains("components")) {
        logging::log(LEVEL_ERROR, "Failed to find components member of prefab!");
        return nullptr;
    }

    auto builder = manager->buildPrefab();
    serializer->deserializePrefab(builder, json.at("components"));

    auto prefab = std::make_unique<Prefab>(builder.build());
    auto* ptr = prefab.get();
    prefabs[id] = std::move(prefab);

    logging::log(LEVEL_DEBUG, "Loaded prefab {}!", id);

    return ptr;
}

void component::PrefabManager::queueUnload (std::size_t id) {
    logging::log(LEVEL_DEBUG, "Unload requested for prefab {}!", id);
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
