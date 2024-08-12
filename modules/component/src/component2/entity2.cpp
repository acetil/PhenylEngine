#include "component2/entity2.h"

#include "component2/component.h"

using namespace phenyl::component;

phenyl::Logger Entity2::LOGGER{"ENTITY2", detail::COMPONENT_LOGGER};

Entity2::Entity2 (EntityId id, ComponentManager2* compManager) : entityId{id}, compManager{compManager} {}

const detail::EntityEntry& Entity2::entry () const {
    PHENYL_DASSERT(exists());
    return compManager->entityEntries[id().pos()];
}

bool Entity2::exists () const noexcept {
    return (bool)entityId && compManager && compManager->exists(entityId);
}

Entity2 Entity2::parent () const {
    return compManager->parent(id());
}

ChildrenView2 Entity2::children() const noexcept {
    return ChildrenView2{entityId, compManager};
}

void Entity2::remove () {
    compManager->remove(entityId);
}
