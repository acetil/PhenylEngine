#include "core/entity.h"

#include "core/detail/loggers.h"
#include "core/world.h"

using namespace phenyl::core;

phenyl::Logger Entity::LOGGER{"ENTITY", phenyl::core::detail::COMPONENT_LOGGER};

Entity::Entity (EntityId id, World* entityWorld) : m_id{id}, m_world{entityWorld} {}

const detail::EntityEntry& Entity::entry () const {
    PHENYL_DASSERT(exists());
    return m_world->m_entityEntries[id().pos()];
}

void Entity::raiseUntyped (std::size_t signalType, std::byte* ptr) {
    m_world->raiseSignal(id(), signalType, ptr);
}

bool Entity::shouldDefer () {
    return m_world->m_deferCount;
}

void Entity::deferInsert (std::size_t compType, std::byte* ptr) {
    m_world->deferInsert(id(), compType, ptr);
}

void Entity::deferErase (std::size_t compType) {
    m_world->deferErase(id(), compType);
}

void Entity::deferApply (std::function<void(Entity)> applyFunc) {
    m_world->deferApply(id(), std::move(applyFunc));
}

bool Entity::exists () const noexcept {
    return (bool) m_id && m_world && m_world->exists(m_id);
}

Entity Entity::parent () const {
    return m_world->parent(id());
}

ChildrenView Entity::children () const noexcept {
    return ChildrenView{m_id, m_world};
}

void Entity::remove () {
    m_world->remove(m_id);
}

void Entity::addChild (Entity child) {
    m_world->reparent(child.id(), id());
}

Entity Entity::createChild () {
    return m_world->create(id());
}
