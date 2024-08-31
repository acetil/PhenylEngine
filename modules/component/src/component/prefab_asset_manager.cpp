#include <iostream>


#include "component/prefab_asset_manager.h"
#include "component/prefab.h"
#include "component/component_serializer.h"
#include "component/detail/loggers.h"

#include "common/serialization/backends.h"
#include "common/assets/assets.h"

using namespace phenyl;

static Logger LOGGER{"PREFAB_ASSET_MANAGER", component::detail::COMPONENT_LOGGER};

class PrefabSerializable : public common::ISerializable<component::Prefab> {
private:
    component::World& world;
    component::EntityComponentSerializer& compSerializer;

    class ChildrenSerializable : public common::ISerializable<component::PrefabBuilder> {
    private:
        PrefabSerializable& prefabSerializable;
    public:
        explicit ChildrenSerializable (PrefabSerializable& prefabSerializable) : prefabSerializable{prefabSerializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Prefab::Children";
        }

        void serialize (common::ISerializer& serializer, const component::PrefabBuilder& obj) override {
            PHENYL_ABORT("Prefab children serialization not supported!");
        }

        void deserialize (common::IDeserializer& deserializer, component::PrefabBuilder& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (component::PrefabBuilder& obj, common::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                component::Prefab child;
                deserializer.next(prefabSerializable, child);
                obj.withChild(child);
            }
        }
    };
public:
    PrefabSerializable (component::World& world, component::EntityComponentSerializer& compSerializer) : world{world}, compSerializer{compSerializer} {}

    std::string_view name () const noexcept override {
        return "phenyl::Prefab";
    }

    void serialize(common::ISerializer& serializer, const component::Prefab& obj) override {
        PHENYL_ABORT("Prefab serialization not supported!");
    }

    void deserialize (common::IDeserializer& deserializer, component::Prefab& obj) override {
        deserializer.deserializeObject(*this, obj);
    }

    void deserializeObject (component::Prefab& obj, common::IObjectDeserializer& deserializer) override {
        auto builder = world.buildPrefab();

        while (deserializer.hasNext()) {
            auto key = deserializer.nextKey();
            if (key == "children") {
                ChildrenSerializable serializable{*this};
                deserializer.nextValue(serializable, builder);
            } else if (key == "components") {
                auto serializable = compSerializer.prefab();
                deserializer.nextValue(serializable, builder);
            } else {
                PHENYL_LOGE(LOGGER, "Unexpected key in prefab deserialization: \"{}\"", key);
                deserializer.ignoreNextValue();
            }
        }

        obj = builder.build();
    }
};

component::Prefab* component::PrefabAssetManager::load (std::ifstream& data, std::size_t id) {
    /*PHENYL_DASSERT(!prefabs.contains(id));
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
    serializer.deserializePrefab(builder, json.at("components"));*/
    PrefabSerializable serializable{world, serializer};

    auto prefab = std::make_unique<Prefab>();
    try {
        common::DeserializeFromJson(data, serializable, *prefab);
    } catch (const phenyl::DeserializeException& e) {
        PHENYL_LOGE(LOGGER, "Failed to deserialize prefab {}: {}", id, e.what());
        return nullptr;
    }

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
