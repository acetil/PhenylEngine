#include "core/entity.h"
#include "core/world.h"
#include "core/detail/loggers.h"

using namespace phenyl::core;

phenyl::Logger Entity::LOGGER{"ENTITY", phenyl::core::detail::COMPONENT_LOGGER};

Entity::Entity (EntityId id, World* entityWorld) : entityId{id}, entityWorld{entityWorld} {}

const detail::EntityEntry& Entity::entry () const {
    PHENYL_DASSERT(exists());
    return entityWorld->entityEntries[id().pos()];
}

void Entity::raiseUntyped (std::size_t signalType, std::byte* ptr) {
    entityWorld->raiseSignal(id(), signalType, ptr);
}

bool Entity::shouldDefer () {
    return entityWorld->deferCount;
}

void Entity::deferInsert(std::size_t compType, std::byte* ptr) {
    entityWorld->deferInsert(id(), compType, ptr);
}

void Entity::deferErase(std::size_t compType) {
    entityWorld->deferErase(id(), compType);
}

void Entity::deferApply(std::function<void(Entity)> applyFunc) {
    entityWorld->deferApply(id(), std::move(applyFunc));
}

bool Entity::exists () const noexcept {
    return (bool)entityId && entityWorld && entityWorld->exists(entityId);
}

Entity Entity::parent () const {
    return entityWorld->parent(id());
}

ChildrenView Entity::children() const noexcept {
    return ChildrenView{entityId, entityWorld};
}

void Entity::remove () {
    entityWorld->remove(entityId);
}

void Entity::addChild (Entity child) {
    entityWorld->reparent(child.id(), id());
}

Entity Entity::createChild () {
    return entityWorld->create(id());
}
