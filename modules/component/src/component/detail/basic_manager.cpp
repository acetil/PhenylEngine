#include "component/detail/managers/basic_manager.h"
#include "component/detail/children_view.h"
#include "component/signals/children_update.h"

using namespace component::detail;

component::ChildrenView BasicManager::_children (EntityId id) {
    return {this, id};
}

component::Entity BasicManager::_entity (component::EntityId id) {
    return Entity{id, this};
}

component::ConstEntity BasicManager::_entity (component::EntityId id) const {
    return _constEntity(id);
}

component::ConstEntity BasicManager::_constEntity (component::EntityId id) const {
    return ConstEntity{id, this};
}

void BasicManager::signalAddChild (component::EntityId id, component::EntityId child) {
    if (id) {
        _signal(id, OnAddChild{.child=_entity(child)});
    }
}

void BasicManager::signalRemoveChild (component::EntityId id, component::EntityId oldChild) {
    if (id) {
        _signal(id, OnRemoveChild{.child=_entity(oldChild)});
    }
}
