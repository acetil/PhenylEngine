#include "component/detail/basic_manager.h"
#include "component/detail/children_view.h"

using namespace component::detail;

component::ChildrenView BasicComponentManager::_children (EntityId id) {
    return {this, id};
}

component::Entity BasicComponentManager::_view (component::EntityId id) {
    return Entity{id, this};
}

component::ConstEntity BasicComponentManager::_view (component::EntityId id) const {
    return ConstEntity{id, this};
}
