#pragma once

#include "core/component/forward.h"
#include "core/entity.h"
#include "core/serialization/component_serializer.h"
#include "core/world.h"

namespace phenyl {
using EntityId = phenyl::core::EntityId;

using World = phenyl::core::World;

using Entity = phenyl::core::Entity;

template <typename... Args>
using Bundle = phenyl::core::Bundle<Args...>;
} // namespace phenyl
