#include "component/detail/managers/basic_manager.h"
#include "component/detail/children_view.h"

using namespace component::detail;

component::ChildrenView BasicManager::_children (EntityId id) {
    return {this, id};
}

component::Entity BasicManager::_entity (component::EntityId id) {
    return Entity{id, this};
}

component::ConstEntity BasicManager::_entity (component::EntityId id) const {
    return ConstEntity{id, this};
}
