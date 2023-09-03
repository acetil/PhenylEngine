#pragma once

#include <cstddef>

namespace util {
    class DataValue;
}

namespace component {
    class EntityId;
    class ComponentManager;
    class EntitySerialiser;
    class EntityComponentFactory;


    template <typename ...Args>
    class EntityComponentView;
    template <typename ...Args>
    class ConstEntityComponentView;

    using EntityComponentManager = ComponentManager;

    class EntityView;
    class ConstEntityView;
    class ChildrenView;

    //using EntitySerialiser = EntitySerialiser;
    //using EntityComponentFactory = EntityComponentFactory;
}