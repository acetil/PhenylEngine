#pragma once

#include <functional>
#include "serialisable_component.h"
#include "component/component.h"
#include "util/data.h"

namespace component {

    template <class CompManager, typename T, std::enable_if_t<std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void serialiseComp (CompManager compManager, util::DataObject& dataObj, component::EntityId id) {
        dataObj[std::string(T::_getName())] = compManager->template getObjectData<T>(id).orElse(T())._serialise();
    }

    template <class CompManager, typename T, std::enable_if_t<!std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void serialiseComp (CompManager compManager, util::DataObject& dataObj, component::EntityId id) {

    }

    template <class CompManager, typename T, typename ...Args, std::enable_if_t<1 <= sizeof...(Args), int> = 0>
    void serialiseComps (CompManager compManager, util::DataObject& dataObj, component::EntityId id) {
        serialiseComp<decltype(compManager), T>(compManager, dataObj, id);
        serialiseComps<decltype(compManager), Args...>(compManager, dataObj, id);
    }

    template <class CompManager, typename T>
    void serialiseComps (CompManager compManager, util::DataObject& dataObj, component::EntityId id) {
        serialiseComp<decltype(compManager), T>(compManager, dataObj, id);
    }

    template <typename ...Args>
    util::DataValue serialise (component::EntityComponentManager::SharedPtr compManager, component::EntityId id) {
        util::DataObject dataObj;
        serialiseComps<decltype(compManager), Args...>(compManager, dataObj, id);
        return util::DataValue(dataObj);
    }

    template <class CompManager, typename T, std::enable_if_t<std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void deserialiseComp (CompManager compManager, component::EntityId id, const util::DataObject& obj) {
        compManager->template getObjectData<T>(id).ifPresent([&obj](auto& t){t._deserialise(obj.at(std::string(T::_getName())));});
    }

    template <class CompManager, typename T, std::enable_if_t<!std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void deserialiseComp (CompManager compManager, component::EntityId id, const util::DataObject& obj) {

    }

    template <class CompManager, typename T, typename ...Args, std::enable_if_t<1 <= sizeof...(Args), int> = 0>
    void deserialiseCompsInt (CompManager compManager, component::EntityId id, const util::DataObject& obj) {
        deserialiseComp<CompManager, T>(compManager, id, obj);
        deserialiseCompsInt<CompManager, Args...>(compManager, id, obj);
    }

    template <class CompManager, typename T>
    void deserialiseCompsInt (CompManager compManager, component::EntityId id, const util::DataObject& obj) {
        deserialiseComp<CompManager, T>(compManager, id, obj);
    }

    template <typename ...Args>
    void deserialiseComps (EntityComponentManager::SharedPtr compManager, EntityId id, const util::DataObject& obj) {
        deserialiseCompsInt<decltype(compManager), Args...>(compManager, id, obj);
    }
}
