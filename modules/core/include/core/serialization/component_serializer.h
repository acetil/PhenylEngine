#pragma once

#include "util/hash.h"
#include "util/map.h"

#include "core/serialization/serializer.h"
#include "core/world.h"
#include "core/prefab.h"

namespace phenyl::core {
    class IComponentSerializer {
    protected:
        std::string componentName;
    public:
        explicit IComponentSerializer (std::string componentName) : componentName{std::move(componentName)} {}
        virtual ~IComponentSerializer () = default;

        [[nodiscard]] std::string_view name () const noexcept {
            return componentName;
        }

        virtual void serialize (IObjectSerializer& serializer, Entity entity) = 0;

        virtual void deserialize (IObjectDeserializer& deserializer, Entity entity) = 0;
        virtual void deserialize (IObjectDeserializer& deserializer, PrefabBuilder& builder) = 0;
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
    private:
        util::HashMap<std::string, std::unique_ptr<IComponentSerializer>> serializers;
        void serializeEntity (ISerializer& serializer, Entity entity) {
            auto& objSerializer = serializer.serializeObj();
            for (auto& [_, comp] : serializers) {
                comp->serialize(objSerializer, entity);
            }
        }

        void deserializeEntity (IObjectDeserializer& deserializer, Entity entity) {
            while (deserializer.hasNext()) {
                auto key = deserializer.nextKey();
                auto it = serializers.find(key);
                if (it == serializers.end()) {
                    throw phenyl::DeserializeException(std::format("Invalid component type: {}", key));
                }

                it->second->deserialize(deserializer, entity);
            }
        }

        void deserializePrefab (IObjectDeserializer& deserializer, PrefabBuilder& builder) {
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
        class PrefabSerializable : public ISerializable<PrefabBuilder> {
        private:
            EntityComponentSerializer& compSerializer;
        public:
            explicit PrefabSerializable (EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}
            std::string_view name () const noexcept override {
                return "phenyl::Prefab::Components";
            }

            void serialize (ISerializer& serializer, const PrefabBuilder& obj) override {
                PHENYL_ABORT("Cannot serialize prefab builder");
            }

            void deserialize (IDeserializer& deserializer, PrefabBuilder& obj) override {
                deserializer.deserializeObject(*this, obj);
            }

            void deserializeObject (PrefabBuilder& obj, IObjectDeserializer& deserializer) override {
                compSerializer.deserializePrefab(deserializer, obj);
            }
        };

        class EntitySerializable : public ISerializable<Entity> {
        private:
            EntityComponentSerializer& compSerializer;
        public:
            explicit EntitySerializable (EntityComponentSerializer& compSerializer) : compSerializer{compSerializer} {}

            std::string_view name () const noexcept override {
                return "phenyl::Entity::Components";
            }

            void serialize (ISerializer& serializer, const Entity& obj) override {
                compSerializer.serializeEntity(serializer, obj);
            }

            void deserialize (IDeserializer& deserializer, Entity& obj) override {
                deserializer.deserializeObject(*this, obj);
            }

            void deserializeObject (Entity& obj, IObjectDeserializer& deserializer) override {
                compSerializer.deserializeEntity(deserializer, obj);
            }
        };

        template <SerializableType T>
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