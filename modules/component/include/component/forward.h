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

    using EntityComponentManager = ComponentManager;

    class Entity;
    class ConstEntity;
    class ChildrenView;
}