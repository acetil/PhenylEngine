#include "core/component/prefab_asset_manager.h"

#include "core/assets/assets.h"
#include "core/detail/loggers.h"
#include "core/prefab.h"
#include "core/serialization/backends.h"
#include "core/serialization/component_serializer.h"

#include <iostream>

using namespace phenyl;

static Logger LOGGER{"PREFAB_ASSET_MANAGER", core::detail::COMPONENT_LOGGER};

class PrefabSerializable : public core::ISerializable<core::Prefab> {
private:
    static constexpr std::string MEMBERS[] = {"children", "components"};

    core::World& world;
    core::EntityComponentSerializer& compSerializer;

    class ChildrenSerializable : public core::ISerializable<core::PrefabBuilder> {
    private:
        PrefabSerializable& prefabSerializable;

    public:
        explicit ChildrenSerializable (PrefabSerializable& prefabSerializable) :
            prefabSerializable{prefabSerializable} {}

        std::string_view name () const noexcept override {
            return "phenyl::Prefab::Children";
        }

        void serialize (core::ISerializer& serializer, const core::PrefabBuilder& obj) override {
            PHENYL_ABORT("Prefab children serialization not supported!");
        }

        void deserialize (core::IDeserializer& deserializer, core::PrefabBuilder& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (core::PrefabBuilder& obj, core::IArrayDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                core::Prefab child;
                deserializer.next(prefabSerializable, child);
                obj.withChild(child);
            }
        }
    };

public:
    PrefabSerializable (core::World& world, core::EntityComponentSerializer& compSerializer) :
        world{world},
        compSerializer{compSerializer} {}

    std::string_view name () const noexcept override {
        return "phenyl::Prefab";
    }

    void serialize (core::ISerializer& serializer, const core::Prefab& obj) override {
        PHENYL_ABORT("Prefab serialization not supported!");
    }

    void deserialize (core::IDeserializer& deserializer, core::Prefab& obj) override {
        deserializer.deserializeStruct(*this, MEMBERS, obj);
    }

    void deserializeStruct (core::Prefab& obj, core::IStructDeserializer& deserializer) override {
        auto builder = world.buildPrefab();

        ChildrenSerializable childrenSerializable{*this};
        deserializer.next("children", childrenSerializable, builder);
        auto compSerializable = compSerializer.prefab();
        deserializer.next("components", compSerializable, builder);

        obj = builder.build();
    }
};

core::Prefab* core::PrefabAssetManager::load (std::ifstream& data, std::size_t id) {
    PrefabSerializable serializable{m_world, m_serializer};

    auto prefab = std::make_unique<Prefab>();
    try {
        core::DeserializeFromJson(data, serializable, *prefab);
    } catch (const phenyl::DeserializeException& e) {
        PHENYL_LOGE(LOGGER, "Failed to deserialize prefab {}: {}", id, e.what());
        return nullptr;
    }

    auto* ptr = prefab.get();
    m_prefabs[id] = std::move(prefab);

    PHENYL_TRACE(LOGGER, "Loaded prefab {}!", id);

    return ptr;
}

std::shared_ptr<core::Prefab> core::PrefabAssetManager::load2 (std::ifstream& data) {
    PrefabSerializable serializable{m_world, m_serializer};

    auto prefab = std::make_shared<Prefab>();
    try {
        core::DeserializeFromJson(data, serializable, *prefab);
    } catch (const phenyl::DeserializeException& e) {
        PHENYL_LOGE(LOGGER, "Failed to deserialize prefab: {}", e.what());
        return nullptr;
    }
    return prefab;
}

void core::PrefabAssetManager::queueUnload (std::size_t id) {
    PHENYL_TRACE(LOGGER, "Unload requested for prefab {}!", id);
}

void core::PrefabAssetManager::selfRegister () {
    core::Assets::AddManager(this);
}

core::PrefabAssetManager::~PrefabAssetManager () {
    core::Assets::RemoveManager(this);
}

const char* core::PrefabAssetManager::getFileType () const {
    return ".json";
}

void core::PrefabAssetManager::clear () {
    m_prefabs.clear();
}

core::Prefab* core::PrefabAssetManager::load (core::Prefab&& obj, std::size_t id) {
    m_prefabs[id] = std::make_unique<core::Prefab>(std::move(obj));
    return m_prefabs[id].get();
}
