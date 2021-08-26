#include <functional>
#include "serialisable_component.h"
#include "component.h"
#include "util/data.h"

#ifndef COMPONENT_SERIALISATION_H
#define COMPONENT_SERIALISATION_H
namespace component {

    template <class CompManager, typename T, std::enable_if_t<std::is_base_of_v<SerialisableComponent<T>, T>, int> = 0>
    void serialiseComp (CompManager compManager, util::DataObject& dataObj, int id) {
        dataObj[T::_getName()] = compManager->template getObjectData<T>(id)._serialise();
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
}
#endif
