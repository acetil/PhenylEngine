#pragma once

#include "common/serializer.h"
#include "component.h"
#include "prefab.h"

namespace phenyl::component {
    class ComponentSerializer {
    public:
        virtual ~ComponentSerializer() = default;

        virtual bool deserialize (component::Prefab::Instantiator& instantiator, const common::JsonDeserializer& deserializer) = 0;
        virtual bool deserialize (component::PrefabBuilder& builder, const common::JsonDeserializer& deserializer) = 0;

        virtual util::Optional<nlohmann::json> serialize (const ConstEntity& entity) = 0;
    };

    namespace detail {
        template <common::Serializable T>
        class TypedComponentSerializer : public ComponentSerializer {
        private:
        public:
            ~TypedComponentSerializer() override = default;

            bool deserialize (component::Prefab::Instantiator& instantiator, const common::JsonDeserializer& deserializer) override {
                /*return deserializer.deserialize<T>()
                    .ifPresent([&instantiator] (T& comp) {
                        instantiator.with(std::move(comp));
                        return true;
                    })
                    .orElse(false);*/
                util::Optional<T> compOpt = deserializer.deserialize<T>();
                if (compOpt) {
                    instantiator.with(std::move(compOpt.getUnsafe()));
                    return true;
                } else {
                    return false;
                }
            }

            bool deserialize (component::PrefabBuilder& builder, const common::JsonDeserializer& deserializer) override {
                /*return deserializer.deserialize<T>()
                    .ifPresent([&builder] (T& comp) {
                        builder.with(std::move(comp));
                        return true;
                    })
                    .orElse(false);*/
                util::Optional<T> compOpt = deserializer.deserialize<T>();
                if (compOpt) {
                    builder.with(std::move(compOpt.getUnsafe()));
                    return true;
                } else {
                    return false;
                }
            }

            util::Optional<nlohmann::json> serialize (const component::ConstEntity& entity) override {
                return entity.get<T>().thenMap([] (const T& comp) {
                    return common::JsonSerializer::Serialize(comp);
                });
            }
        };
    }

    class EntitySerializer {
    private:
        util::Map<std::string, std::unique_ptr<ComponentSerializer>> serializers;
    public:
        template <common::CustomSerializable T>
        void addSerializer () {
            if (serializers.contains(common::CustomSerializer<T>::Name)) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Attempted to add serializer \"{}\" that already exists!", common::CustomSerializer<T>::Name);
                return;
            }

            serializers[common::CustomSerializer<T>::Name] = std::make_unique<detail::TypedComponentSerializer<T>>();
        }

        nlohmann::json serializeEntity (const component::ConstEntity& entity) {
            auto obj = nlohmann::json::object_t{};

            for (auto [compId, serializer] : serializers.kv()) {
                serializer->serialize(entity).ifPresent([&obj, &compId] (nlohmann::json& data) {
                    obj[compId] = std::move(data);
                });
            }

            return obj;
        }

        void deserializeEntity (component::Prefab::Instantiator& instantiator, const nlohmann::json& data) {
            if (!data.is_object()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Entity component data is not an object!");
                return;
            }

            const auto& obj = data.get<nlohmann::json::object_t>();

            for (const auto& [k, v] : obj) {
                if (serializers.contains(k)) {
                    serializers.at(k)->deserialize(instantiator, v);
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
                    serializers.at(k)->deserialize(builder, v);
                } else {
                    PHENYL_LOGW(detail::SERIALIZER_LOGGER, "Component serializer not available for component \"{}\"!", k);
                }
            }
        }
    };
}