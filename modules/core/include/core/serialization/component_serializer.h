#pragma once

#include "core/detail/loggers.h"
#include "core/prefab.h"
#include "core/serialization/serializer.h"
#include "core/world.h"
#include "debug_schema.h"
#include "schema.h"
#include "util/hash.h"

namespace phenyl::core {
class IComponentSerializer {
public:
    explicit IComponentSerializer (std::string componentName) : m_name{std::move(componentName)} {}

    virtual ~IComponentSerializer () = default;

    [[nodiscard]] std::string_view name () const noexcept {
        return m_name;
    }

    virtual void serialize (IObjectSerializer& serializer, Entity entity) = 0;

    virtual void deserialize (IObjectDeserializer& deserializer, Entity entity) = 0;
    virtual void deserialize (IObjectDeserializer& deserializer, PrefabBuilder& builder) = 0;

protected:
    std::string m_name;
};

template <SerializableType T>
class ComponentSerializer : public IComponentSerializer {
public:
    ComponentSerializer () : IComponentSerializer{std::string{detail::GetSerializable<T>().name()}} {}

    void serialize (IObjectSerializer& serializer, Entity entity) override {
        const T* ptr = entity.get<const T>();
        if (ptr) {
            serializer.serializeMember(name(), *ptr);
        }
    }

    void deserialize (IObjectDeserializer& deserializer, Entity entity) override {
        entity.insert(deserializer.nextValue<T>());
    }

    void deserialize (IObjectDeserializer& deserializer, PrefabBuilder& builder) override {
        builder.with(deserializer.nextValue<T>());
    }
};

class EntityComponentSerializer {
public:
    class PrefabSerializable : public ISerializable<PrefabBuilder> {
    private:
        EntityComponentSerializer& m_serializer;

    public:
        explicit PrefabSerializable (EntityComponentSerializer& compSerializer) : m_serializer{compSerializer} {}

        std::string_view name () const noexcept override {
            return "phenyl::Prefab::Components";
        }

        void serialize (ISerializer& serializer, const PrefabBuilder& obj) override {
            PHENYL_ABORT("Cannot serialize prefab builder");
        }

        void accept (ISchemaVisitor& visitor) override {
            visitor.visitEngineType(EngineSerializableType::Prefab);
        }

        void deserialize (IDeserializer& deserializer, PrefabBuilder& obj) override {
            deserializer.deserializeObject(*this, obj);
        }

        void deserializeObject (PrefabBuilder& obj, IObjectDeserializer& deserializer) override {
            m_serializer.deserializePrefab(deserializer, obj);
        }
    };

    class EntitySerializable : public ISerializable<Entity> {
    private:
        EntityComponentSerializer& m_serializer;

    public:
        explicit EntitySerializable (EntityComponentSerializer& compSerializer) : m_serializer{compSerializer} {}

        std::string_view name () const noexcept override {
            return "phenyl::Entity::Components";
        }

        void serialize (ISerializer& serializer, const Entity& obj) override {
            m_serializer.serializeEntity(serializer, obj);
        }

        void accept (ISchemaVisitor& visitor) override {
            visitor.visitEngineType(EngineSerializableType::Entity);
        }

        void deserialize (IDeserializer& deserializer, Entity& obj) override {
            deserializer.deserializeObject(*this, obj);
        }

        void deserializeObject (Entity& obj, IObjectDeserializer& deserializer) override {
            m_serializer.deserializeEntity(deserializer, obj);
        }
    };

    template <SerializableType T>
    void addSerializer () {
        auto serializer = std::make_unique<ComponentSerializer<T>>();
        m_serializers.emplace(serializer->name(), std::move(serializer));

        std::stringstream debugStr;
        DebugSchemaVisitor debugVisitor{debugStr};
        static_cast<ISchemaVisitor&>(debugVisitor).visit<T>();
        PHENYL_LOGD(detail::SERIALIZER_LOGGER, "Added serializer: \n{}", debugStr.str());
    }

    PrefabSerializable prefab () {
        return PrefabSerializable{*this};
    }

    EntitySerializable entity () {
        return EntitySerializable{*this};
    }

private:
    util::HashMap<std::string, std::unique_ptr<IComponentSerializer>> m_serializers;

    void serializeEntity (ISerializer& serializer, Entity entity) {
        auto& objSerializer = serializer.serializeObj();
        for (auto& [_, comp] : m_serializers) {
            comp->serialize(objSerializer, entity);
        }
    }

    void deserializeEntity (IObjectDeserializer& deserializer, Entity entity) {
        while (deserializer.hasNext()) {
            auto key = deserializer.nextKey();
            auto it = m_serializers.find(key);
            if (it == m_serializers.end()) {
                throw phenyl::DeserializeException(std::format("Invalid component type: {}", key));
            }

            it->second->deserialize(deserializer, entity);
        }
    }

    void deserializePrefab (IObjectDeserializer& deserializer, PrefabBuilder& builder) {
        while (deserializer.hasNext()) {
            auto key = deserializer.nextKey();
            auto it = m_serializers.find(key);
            if (it == m_serializers.end()) {
                throw phenyl::DeserializeException(std::format("Invalid component type: {}", key));
            }

            it->second->deserialize(deserializer, builder);
        }
    }
};
} // namespace phenyl::core
