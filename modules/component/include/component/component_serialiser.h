#pragma once

#include <string>

#include "util/map.h"
#include "util/data.h"
#include "util/optional.h"
#include "component.h"

namespace component {
    namespace detail {
        //template <size_t MaxComponents>
        class ComponentSerialiser {
        public:
            virtual ~ComponentSerialiser () = default;

            virtual bool deserialiseComp (component::EntityView& entityView, const util::DataValue& serialisedComp) = 0;

            virtual util::DataValue serialiseComp (component::EntityView& entityView) = 0;

            virtual bool hasComp (component::EntityView& entityView) const = 0;
        };

        template</*size_t MaxComponents, */class T, typename SerialiseF, typename DeserialiseF>
        class ComponentSerialiserImpl : public ComponentSerialiser/*<MaxComponents>*/ {
        private:
            SerialiseF serialiseF; // (const T&) -> util::DataValue
            DeserialiseF deserialiseF; // (const util::DataValue&) -> util::Optional<T>
        public:
            ComponentSerialiserImpl (SerialiseF f1, DeserialiseF f2) : serialiseF{f1}, deserialiseF{f2} {}

            util::DataValue serialiseComp (/*component::ComponentView<MaxComponents>*/component::EntityView& objectView) override {
                return objectView.get<T>()
                        .thenMap([this] (T& comp) -> util::DataValue {
                             return serialiseF(comp);
                         })
                         .orElse({});
            }

            bool deserialiseComp (/*component::ComponentView<MaxComponents>*/component::EntityView& objectView, const util::DataValue& serialisedComp) override {
                auto opt = deserialiseF(serialisedComp);

                opt.ifPresent([&objectView] (T& val) {
                    objectView.insert<T>(std::move(val));
                });

                return opt;
            }

            bool hasComp (/*component::ComponentView<MaxComponents>*/component::EntityView& objectView) const override {
                return objectView.has<T>();
            }
        };
    }


    //template <size_t MaxComponents>
    class ObjectComponentFactory {
    private:
        detail::ComponentSerialiser/*<MaxComponents>*/* serialiser;
        util::DataValue compData;
    public:
        ObjectComponentFactory (detail::ComponentSerialiser/*<MaxComponents>*/* serialiser, util::DataValue compData) : serialiser{serialiser}, compData{std::move(compData)} {}

        void addDefault (/*component::ComponentView<MaxComponents>*/EntityView& objectView) const {
            if (!serialiser->hasComp(objectView)) {
                serialiser->deserialiseComp(objectView, compData);
            }
        }
    };

    //template <size_t MaxComponents>
    class ObjectSerialiser {
    private:
        util::Map<std::string, std::unique_ptr<detail::ComponentSerialiser/*<MaxComponents>*/>> serialiserMap;
        void addComponentSerialiserInt (const std::string& component, std::unique_ptr<detail::ComponentSerialiser/*<MaxComponents>*/> serialiser) {
            serialiserMap[component] = std::move(serialiser);
        }
    public:
        template <class T, typename F1, typename F2>
        void addComponentSerialiser (const std::string& component, F1 serialiseFunc, F2 deserialiseFunc) {
            addComponentSerialiserInt(component, std::make_unique<detail::ComponentSerialiserImpl</*MaxComponents, */T, F1, F2>>(std::move(serialiseFunc), std::move(deserialiseFunc)));
        }

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

        util::DataObject serialiseObject (component::EntityView/*<MaxComponents>*/& entityView) const {
            util::DataObject dataObj;

            for (const auto& [compId, serialiser] : serialiserMap.kv()) {
                util::DataValue val = serialiser->serialiseComp(entityView);

                if (!val.empty()) {
                    dataObj[compId] = std::move(val);
                }
            }

            return dataObj;
        }

        void deserialiseObject (component::EntityView& entityView, const util::DataValue& dataVal) const {
            if (!dataVal.is<util::DataObject>()) {
                logging::log(LEVEL_WARNING, "Entity data is not an object!");
                return;
            }

            const auto& dataObj = dataVal.get<util::DataObject>();

            for (const auto& [compId, serialiser] : serialiserMap.kv()) {
                if (dataObj.contains(compId)) {
                    serialiser->deserialiseComp(entityView, dataObj.at(compId));
                }
            }
        }

        util::Optional<component::ObjectComponentFactory/*<MaxComponents>*/> makeFactory (const std::string& component, util::DataValue dataVal) const {
            if (serialiserMap.contains(component)) {
                return {{serialiserMap.at(component).get(), dataVal}};
            } else {
                return util::NullOpt;
            }
        }
    };

    using EntitySerialiser = ObjectSerialiser;
    //extern template class ObjectSerialiser<PHENYL_MAX_COMPONENTS>;
    //extern template class ObjectComponentFactory<PHENYL_MAX_COMPONENTS>;
}