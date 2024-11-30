#pragma once

#include "core/signals/component_update.h"
#include "core/signals/children_update.h"

namespace phenyl::signals {
    template <typename T>
    using OnInsert = phenyl::core::OnInsert<T>;
    template <typename T>
    using OnRemove = phenyl::core::OnRemove<T>;

    using OnAddChild = phenyl::core::OnAddChild;
    using OnRemoveChild = phenyl::core::OnRemoveChild;
}