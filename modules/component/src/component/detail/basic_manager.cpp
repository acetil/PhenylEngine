#include "component/detail/managers/basic_manager.h"
#include "component/detail/children_view.h"
#include "component/signals/children_update.h"

using namespace phenyl::component::detail;

phenyl::component::ChildrenView BasicManager::_children (EntityId id) {
    return {this, id};
}

phenyl::component::Entity BasicManager::_entity (component::EntityId id) {
    return Entity{id, this};
}

phenyl::component::ConstEntity BasicManager::_entity (component::EntityId id) const {
    return _constEntity(id);
}

phenyl::component::ConstEntity BasicManager::_constEntity (component::EntityId id) const {
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
