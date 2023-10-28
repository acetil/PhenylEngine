module;

#include "component/component.h"
#include "component/entity.h"
#include "component/forward.h"
#include "component/component_serializer.h"

export module phenyl.component;

export namespace phenyl {
    using EntityId = phenyl::component::EntityId;

    using ComponentManager = phenyl::component::ComponentManager;
    using ComponentSerializer = phenyl::component::EntitySerializer;

    using Entity = phenyl::component::Entity;
    using ConstEntity = phenyl::component::ConstEntity;
}