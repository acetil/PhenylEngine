#pragma once

#include <string>
#include <concepts>
#include <utility>

#include "util/map.h"
#include "util/data.h"
#include "util/optional.h"
#include "component.h"
#include "util/meta.h"
#include "component/prefab.h"

namespace component {
    namespace detail {
        class ComponentSerialiser {
        public:
            virtual ~ComponentSerialiser () = default;

            virtual bool deserialiseComp (component::Prefab::Instantiator& instantiator, const util::DataValue& serialisedComp) = 0;
            virtual bool deserialiseComp (component::PrefabBuilder& builder, const util::DataValue& serialisedComp) = 0;

            virtual util::DataValue serialiseComp (component::Entity& entityView) = 0;

            virtual bool hasComp (component::Entity& entityView) const = 0;
        };

        template<class T, meta::callable<util::DataValue, const T&> SerialiseF, meta::callable<util::Optional<T>, const util::DataValue&> DeserialiseF>
        class ComponentSerialiserImpl : public ComponentSerialiser {
        private:
            SerialiseF serialiseF; // (const T&) -> util::DataValue
            DeserialiseF deserialiseF; // (const util::DataValue&) -> util::Optional<T>
        public:
            ComponentSerialiserImpl (SerialiseF f1, DeserialiseF f2) : serialiseF{f1}, deserialiseF{f2} {}

            util::DataValue serialiseComp (component::Entity& objectView) override {
                return objectView.get<T>()
                        .thenMap([this] (T& comp) -> util::DataValue {
                             return serialiseF(comp);
                         })
                         .orElse({});
            }

            bool deserialiseComp (component::Prefab::Instantiator& instantiator, const util::DataValue& serialisedComp) override {
                auto opt = deserialiseF(serialisedComp);

                opt.ifPresent([&instantiator] (T& val) {
                    instantiator.with(std::move(val));
                });

                return opt;
            }

            bool deserialiseComp (component::PrefabBuilder& builder, const util::DataValue& serialisedComp) override {
                auto opt = deserialiseF(serialisedComp);

                opt.ifPresent([&builder] (T& val) {
                    builder.with(std::move(val));
                });

                return opt;
            }

            bool hasComp (component::Entity& objectView) const override {
                return objectView.has<T>();
            }
        };
    }


    class EntityComponentFactory {
    private:
        detail::ComponentSerialiser* serialiser;
        util::DataValue compData;
    public:
        //EntityComponentFactory (detail::ComponentSerialiser* serialiser, util::DataValue compData) : serialiser{serialiser}, compData{std::move(compData)} {}

        /*void addDefault (Entity& objectView) const {
            if (!serialiser->hasComp(objectView)) {
                serialiser->deserialiseComp(objectView, compData);
            }
        }*/
    };

    class EntitySerialiser {
    private:
        util::Map<std::string, std::unique_ptr<detail::ComponentSerialiser>> serialiserMap;
        void addComponentSerialiserInt (const std::string& component, std::unique_ptr<detail::ComponentSerialiser> serialiser) {
            serialiserMap[component] = std::move(serialiser);
        }
    public:
        template <class T, meta::callable<util::DataValue, const T&> F1, meta::callable<util::Optional<T>, const util::DataValue&> F2>
        void addComponentSerialiser (const std::string& component, F1 serialiseFunc, F2 deserialiseFunc) {
            addComponentSerialiserInt(component, std::make_unique<detail::ComponentSerialiserImpl<T, F1, F2>>(std::move(serialiseFunc), std::move(deserialiseFunc)));
        }

        /*template <class T, meta::callable<util::DataValue, const T&> F1, meta::callable<util::Optional<T>, const util::DataValue&, EntityId> F2>
        void addComponentSerialiser (const std::string& component, F1 serialiseFunc, F2 deserialiseFunc) {
            addComponentSerialiserInt(component, std::make_unique<detail::ComponentSerialiserImpl<T, F1, F2>>(std::move(serialiseFunc), std::move(deserialiseFunc)));
        }*/

        template <class T>
        void addComponentSerialiser (const std::string& component) {
            addComponentSerialiser<T>(component, [] (const T& comp) -> util::DataValue {
                return phenyl_to_data(comp);
            }, [] (const util::DataValue& val) -> util::Optional<T> {
                T comp{};
                if (phenyl_from_data(val, comp)) {
                    return {comp};
                } else {
                    return util::NullOpt;
                }
            });
        }

        util::DataObject serialiseObject (component::Entity& entityView) const {
            util::DataObject dataObj;

            for (const auto& [compId, serialiser] : serialiserMap.kv()) {
                util::DataValue val = serialiser->serialiseComp(entityView);

                if (!val.empty()) {
                    dataObj[compId] = std::move(val);
                }
            }

            return dataObj;
        }

        void deserialiseObject (component::Prefab::Instantiator& instantiator, const util::DataValue& dataVal) const {
            if (!dataVal.is<util::DataObject>()) {
                logging::log(LEVEL_WARNING, "Entity data is not an object!");
                return;
            }

            const auto& dataObj = dataVal.get<util::DataObject>();

            /*for (const auto& [compId, serialiser] : serialiserMap.kv()) {
                if (dataObj.contains(compId)) {
                    serialiser->deserialiseComp(entityView, dataObj.at(compId));
                }
            }*/
            for (const auto& [compId, compVal] : dataObj.kv()) {
                if (serialiserMap.contains(compId)) {
                    serialiserMap.at(compId)->deserialiseComp(instantiator, compVal);
                } else {
                    logging::log(LEVEL_WARNING, "Component serialiser not available for component \"{}\"!", compId);
                }
            }
        }

        void deserialisePrefab (component::PrefabBuilder& builder, const util::DataValue& dataVal) const {
            if (!dataVal.is<util::DataObject>()) {
                logging::log(LEVEL_WARNING, "Entity data is not an object!");
                return;
            }

            const auto& dataObj = dataVal.get<util::DataObject>();

            /*for (const auto& [compId, serialiser] : serialiserMap.kv()) {
                if (dataObj.contains(compId)) {
                    serialiser->deserialiseComp(entityView, dataObj.at(compId));
                }
            }*/
            for (const auto& [compId, compVal] : dataObj.kv()) {
                if (serialiserMap.contains(compId)) {
                    serialiserMap.at(compId)->deserialiseComp(builder, compVal);
                } else {
                    logging::log(LEVEL_WARNING, "Component serialiser not available for component \"{}\"!", compId);
                }
            }
        }

        /*[[nodiscard]] util::Optional<component::EntityComponentFactory> makeFactory (const std::string& component, util::DataValue dataVal) const {
            if (serialiserMap.contains(component)) {
                return {{serialiserMap.at(component).get(), std::move(dataVal)}};
            } else {
                logging::log(LEVEL_WARNING, "Component serialiser not available for component \"{}\"!", component);
                return util::NullOpt;
            }
        }*/
    };
}