#pragma once

#include "component/signals/component_update.h"
#include "component/signals/children_update.h"

namespace phenyl::signals {
    template <typename T>
    using OnInsert = phenyl::component::OnInsert<T>;
    template <typename T>
    using OnRemove = phenyl::component::OnRemove<T>;

    using OnAddChild = phenyl::component::OnAddChild;
    using OnRemoveChild = phenyl::component::OnRemoveChild;
}