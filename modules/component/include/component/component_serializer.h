#pragma once

#include <nlohmann/json.hpp>

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
    class ComponentSerializer2 : public IComponentSerializer {
    public:
        ComponentSerializer2 () : IComponentSerializer{std::string{common::detail::GetSerializable<T>().name()}} {}

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

    class ComponentSerializer {
    public:
        virtual ~ComponentSerializer() = default;

        // virtual bool deserialize (component::Entity entity, const common::JsonDeserializer& deserializer) = 0;
        // virtual bool deserialize (component::PrefabBuilder& builder, const common::JsonDeserializer& deserializer) = 0;

        virtual util::Optional<nlohmann::json> serialize (const Entity& entity) = 0;
    };

    namespace detail {
        template <common::SerializableType T>
        class TypedComponentSerializer : public ComponentSerializer {
        private:
        public:
            ~TypedComponentSerializer() override = default;

            // bool deserialize (component::Entity entity, const common::JsonDeserializer& deserializer) override {
            //     /*return deserializer.deserialize<T>()
            //         .ifPresent([&instantiator] (T& comp) {
            //             instantiator.with(std::move(comp));
            //             return true;
            //         })
            //         .orElse(false);*/
            //    /* util::Optional<T> compOpt = deserializer.deserialize<T>();
            //     if (compOpt) {
            //         entity.emplace<T>(std::move(compOpt.getUnsafe()));
            //         return true;
            //     } else {
            //         return false;
            //     }*/
            //     return false;
            // }
            //
            // bool deserialize (component::PrefabBuilder& builder, const common::JsonDeserializer& deserializer) override {
            //     /*return deserializer.deserialize<T>()
            //         .ifPresent([&builder] (T& comp) {
            //             builder.with(std::move(comp));
            //             return true;
            //         })
            //         .orElse(false);*/
            //     /*util::Optional<T> compOpt = deserializer.deserialize<T>();
            //     if (compOpt) {
            //         builder.with(std::move(compOpt.getUnsafe()));
            //         return true;
            //     } else {
            //         return false;
            //     }*/
            //     return false;
            // }

            util::Optional<nlohmann::json> serialize (const component::Entity& entity) override {
                /*auto* ptr = entity.get<T>();
                return ptr ? util::Optional<nlohmann::json>{common::JsonSerializer::Serialize(*ptr)} : util::NullOpt;*/
                return util::NullOpt;
            }
        };
    }


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
            auto serializer = std::make_unique<ComponentSerializer2<T>>();
            serializers.emplace(serializer->name(), std::move(serializer));
        }

        PrefabSerializable prefab () {
            return PrefabSerializable{*this};
        }

        EntitySerializable entity () {
            return EntitySerializable{*this};
        }
    };

    class EntitySerializer {
    private:
        util::Map<std::string, std::unique_ptr<ComponentSerializer>> serializers;
    public:
        template <common::SerializableType T>
        void addSerializer () {

        }

        nlohmann::json serializeEntity (const component::Entity& entity) {
            auto obj = nlohmann::json::object_t{};

            for (auto [compId, serializer] : serializers.kv()) {
                serializer->serialize(entity).ifPresent([&obj, &compId] (nlohmann::json& data) {
                    obj[compId] = std::move(data);
                });
            }

            return obj;
        }

        void deserializeEntity (component::Entity entity, const nlohmann::json& data) {
            if (!data.is_object()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Entity component data is not an object!");
                return;
            }

            const auto& obj = data.get<nlohmann::json::object_t>();

            for (const auto& [k, v] : obj) {
                if (serializers.contains(k)) {
                    //serializers.at(k)->deserialize(entity, v);
                } else {
                    PHENYL_LOGW(detail::SERIALIZER_LOGGER, "Component serializer not available for component \"{}\"!", k);
                }
            }
        }

        void deserializePrefab (component::PrefabBuilder& builder, const nlohmann::json& data) {
            if (!data.is_object()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Entity component data is not an object!");
                return;
            }

            const auto& obj = data.get<nlohmann::json::object_t>();

            for (const auto& [k, v] : obj) {
                if (serializers.contains(k)) {
                    //serializers.at(k)->deserialize(builder, v);
                } else {
                    PHENYL_LOGW(detail::SERIALIZER_LOGGER, "Component serializer not available for component \"{}\"!", k);
                }
            }
        }
    };
}