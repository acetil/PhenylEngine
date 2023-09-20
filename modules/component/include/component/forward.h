#pragma once

#include <cstddef>

namespace phenyl::component {
    class EntityId;
    class ComponentManager;
    class EntitySerializer;
    class EntityComponentFactory;

    using EntityComponentManager = ComponentManager;

    class Entity;
    class ConstEntity;
    class ChildrenView;
    class Prefab;
}