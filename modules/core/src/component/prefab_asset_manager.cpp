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

        void accept (core::ISchemaVisitor& visitor) override {
            visitor.visitEngineType(core::EngineSerializableType::Children);
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

    void accept (core::ISchemaVisitor& visitor) override {
        visitor.visitEngineType(core::EngineSerializableType::Prefab);
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

std::shared_ptr<core::Prefab> core::PrefabAssetManager::load (AssetLoadContext& ctx) {
    PrefabSerializable serializable{m_world, m_serializer};
    return ctx.withExtension(".json").deserialize(serializable);
}

void core::PrefabAssetManager::selfRegister () {
    core::Assets::AddManager(this);
}

core::PrefabAssetManager::~PrefabAssetManager () {
    core::Assets::RemoveManager(this);
}
