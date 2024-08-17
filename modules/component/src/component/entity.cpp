#include "component/entity.h"
#include "component/component.h"

using namespace phenyl::component;

phenyl::Logger Entity::LOGGER{"ENTITY", detail::COMPONENT_LOGGER};

Entity::Entity (EntityId id, ComponentManager* compManager) : entityId{id}, compManager{compManager} {}

const detail::EntityEntry& Entity::entry () const {
    PHENYL_DASSERT(exists());
    return compManager->entityEntries[id().pos()];
}

void Entity::raiseUntyped (std::size_t signalType, const std::byte* ptr) {
    compManager->raiseSignal(*this, signalType, ptr);
}

bool Entity::exists () const noexcept {
    return (bool)entityId && compManager && compManager->exists(entityId);
}

Entity Entity::parent () const {
    return compManager->parent(id());
}

ChildrenView Entity::children() const noexcept {
    return ChildrenView{entityId, compManager};
}

void Entity::remove () {
    compManager->remove(entityId);
}

void Entity::addChild (Entity child) {
    compManager->reparent(child.id(), id());
}
