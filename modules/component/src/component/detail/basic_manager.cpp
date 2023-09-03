#include "component/detail/basic_manager.h"
#include "component/detail/children_view.h"

using namespace component::detail;

component::ChildrenView BasicComponentManager::_children (EntityId id) {
    return {this, id};
}

component::EntityView BasicComponentManager::_view (component::EntityId id) {
    return EntityView{id, this};
}

component::ConstEntityView BasicComponentManager::_view (component::EntityId id) const {
    return ConstEntityView{id, this};
}
