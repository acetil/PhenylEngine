#pragma once

#include "util/hash.h"
#include "util/map.h"

#include "common/serialization/serializer.h"
#include "component/component.h"
#include "component/prefab.h"

namespace phenyl::component {
    class IComponentSerializer {
    protected:
        std::string componentName;
    public:
        explicit IComponentSerializer (std::string componentName) : componentName{std::move(componentName)} {}
        virtual ~IComponentSerializer () = default;

        [[nodiscard]] std::string_view name () const noexcept {
            return componentName;
        }

        virtual void serialize (common::IObjectSerializer& serializer, Entity entity) = 0;

        virtual void deserialize (common::IObjectDeserializer& deserializer, Entity entity) = 0;
        virtual void deserialize (common::IObjectDeserializer& deserializer, PrefabBuilder& builder) = 0;
    };

    template <common::SerializableType T>
    class ComponentSerializer : public IComponentSerializer {
    public:
        ComponentSerializer () : IComponentSerializer{std::string{common::detail::GetSerializable<T>().name()}} {}

        void serialize (common::IObjectSerializer& serializer, Entity entity) override {
            const T* ptr = entity.get<const T>();
            if (ptr) {
                serializer.serializeMember(name(), *ptr);
            }
        }

        void deserialize (common::IObjectDeserializer& deserializer, Entity entity) override {
            entity.insert(deserializer.nextValue<T>());
        }

        void deserialize (common::IObjectDeserializer& deserializer, PrefabBuilder& builder) override {
            builder.with(deserializer.nextValue<T>());
        }
    };

    class EntityComponentSerializer {
    private:
        util::HashMap<std::string, std::unique_ptr<IComponentSerializer>> serializers;
        void serializeEntity (common::ISerializer& serializer, Entity entity) {
            auto& objSerializer = serializer.serializeObj();
            for (auto& [_, comp] : serializers) {
                comp->serialize(objSerializer, entity);
            }
        }

        void deserializeEntity (common::IObjectDeserializer& deserializer, Entity entity) {
            while (deserializer.hasNext()) {
                auto key = deserializer.nextKey();
                auto it = serializers.find(key);
                if (it == serializers.end()) {
                    throw phenyl::DeserializeException(std::format("Invalid component type: {}", key));
                }

                it->second->deserialize(deserializer, entity);
            }
        }

        void deserializePrefab (common::IObjectDeserializer& deserializer, PrefabBuilder& builder) {
            while (deserializer.hasNext()) {
                auto key = deserializer.nextKey();
                auto it = serializers.find(key);
                if (it == serializers.end()) {
                    throw phenyl::DeserializeException(std::format("Invalid component type: {}", key));
                }

                it->second->deserialize(deserializer, builder);
            }
        }
    public:
        class PrefabSerializable : public common::ISerializable<PrefabBuilder> {
        private:
            EntityComponentSerializer& compSerializer;
        public:
            explicit PrefabSerializable (EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}
            std::string_view name () const noexcept override {
                return "phenyl::Prefab::Components";
            }

            void serialize (common::ISerializer& serializer, const PrefabBuilder& obj) override {
                PHENYL_ABORT("Cannot serialize prefab builder");
            }

            void deserialize (common::IDeserializer& deserializer, PrefabBuilder& obj) override {
                deserializer.deserializeObject(*this, obj);
            }

            void deserializeObject (PrefabBuilder& obj, common::IObjectDeserializer& deserializer) override {
                compSerializer.deserializePrefab(deserializer, obj);
            }
        };

        class EntitySerializable : public common::ISerializable<Entity> {
        private:
            EntityComponentSerializer& compSerializer;
        public:
            explicit EntitySerializable (EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}

            std::string_view name () const noexcept override {
                return "phenyl::Entity::Components";
            }

            void serialize (common::ISerializer& serializer, const Entity& obj) override {
                compSerializer.serializeEntity(serializer, obj);
            }

            void deserialize (common::IDeserializer& deserializer, Entity& obj) override {
                deserializer.deserializeObject(*this, obj);
            }

            void deserializeObject (Entity& obj, common::IObjectDeserializer& deserializer) override {
                compSerializer.deserializeEntity(deserializer, obj);
            }
        };

        template <common::SerializableType T>
        void addSerializer () {
            auto serializer = std::make_unique<ComponentSerializer<T>>();
            serializers.emplace(serializer->name(), std::move(serializer));
        }

        PrefabSerializable prefab () {
            return PrefabSerializable{*this};
        }

        EntitySerializable entity () {
            return EntitySerializable{*this};
        }
    };
}