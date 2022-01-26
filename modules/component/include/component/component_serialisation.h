#include <functional>
#include "serialisable_component.h"
#include "component/component.h"
#include "util/data.h"

#ifndef COMPONENT_SERIALISATION_H
#define COMPONENT_SERIALISATION_H
namespace component {

    template <class CompManager, typename T, std::enable_if_t<std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void serialiseComp (CompManager compManager, util::DataObject& dataObj, int id) {
        dataObj[std::string(T::_getName())] = compManager->template getObjectData<T>(id)._serialise();
    }

    template <class CompManager, typename T, std::enable_if_t<!std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void serialiseComp (CompManager compManager, util::DataObject& dataObj, int id) {

    }

    template <class CompManager, typename T, typename ...Args, std::enable_if_t<1 <= sizeof...(Args), int> = 0>
    void serialiseComps (CompManager compManager, util::DataObject& dataObj, int id) {
        serialiseComp<decltype(compManager), T>(compManager, dataObj, id);
        serialiseComps<decltype(compManager), Args...>(compManager, dataObj, id);
    }

    template <class CompManager, typename T>
    void serialiseComps (CompManager compManager, util::DataObject& dataObj, int id) {
        serialiseComp<decltype(compManager), T>(compManager, dataObj, id);
    }

    template <typename ...Args>
    util::DataValue serialise (std::shared_ptr<ComponentManagerImpl<Args...>> compManager, int id) {
        util::DataObject dataObj;
        serialiseComps<decltype(compManager), Args...>(compManager, dataObj, id);
        return util::DataValue(dataObj);
    }

    template <class CompManager, typename T, std::enable_if_t<std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void deserialiseComp (CompManager compManager, int id, const util::DataObject& obj) {
        compManager->template getObjectData<T>(id)._deserialise(obj.at(std::string(T::_getName())));
    }

    template <class CompManager, typename T, std::enable_if_t<!std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void deserialiseComp (CompManager compManager, int id, const util::DataObject& obj) {

    }

    template <class CompManager, typename T, typename ...Args, std::enable_if_t<1 <= sizeof...(Args), int> = 0>
    void deserialiseCompsInt (CompManager compManager, int id, const util::DataObject& obj) {
        deserialiseComp<CompManager, T>(compManager, id, obj);
        deserialiseCompsInt<CompManager, Args...>(compManager, id, obj);
    }

    template <class CompManager, typename T>
    void deserialiseCompsInt (CompManager compManager, int id, const util::DataObject& obj) {
        deserialiseComp<CompManager, T>(compManager, id, obj);
    }

    template <typename ...Args>
    void deserialiseComps (std::shared_ptr<ComponentManagerImpl<Args...>> compManager, int id, const util::DataObject& obj) {
        deserialiseCompsInt<decltype(compManager), Args...>(compManager, id, obj);
    }
}
#endif
