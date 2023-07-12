#pragma once

#include <cstddef>

namespace util {
    class DataValue;
}

namespace component {
    class EntityId;
    class ComponentManagerNew;
    //template <size_t MaxComponents>
    class ObjectSerialiser;
    //template <size_t MaxComponents>
    class ObjectComponentFactory;


    template <typename ...Args>
    class EntityComponentView;
    template <typename ...Args>
    class ConstEntityComponentView;

    //using EntityComponentManager = ComponentManager<PHENYL_MAX_COMPONENTS>;
    using EntityComponentManager = ComponentManagerNew;
    //using EntityView = ComponentView<PHENYL_MAX_COMPONENTS>;

    using EntitySerialiser = ObjectSerialiser/*<PHENYL_MAX_COMPONENTS>*/;
    using EntityComponentFactory = ObjectComponentFactory/*<PHENYL_MAX_COMPONENTS>*/;
}