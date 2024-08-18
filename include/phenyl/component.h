#pragma once

#include "component/component.h"
#include "component/entity.h"
#include "component/forward.h"
#include "component/component_serializer.h"

namespace phenyl {
    using EntityId = phenyl::component::EntityId;

    using World = phenyl::component::World;
    using ComponentSerializer = phenyl::component::EntitySerializer;

    using Entity = phenyl::component::Entity;

    template <typename ...Args>
    using Bundle = phenyl::component::Bundle<Args...>;
}