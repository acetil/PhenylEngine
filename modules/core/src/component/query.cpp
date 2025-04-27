#include "core/component/query.h"

#include "core/world.h"

using namespace phenyl::core;

QueryArchetypes::QueryArchetypes (World& world, detail::ArchetypeKey key) : m_world{world}, m_key{std::move(key)} {}

void QueryArchetypes::onNewArchetype (Archetype* archetype) {
    if (archetype->getKey().subsetOf(m_key)) {
        // All components found
        m_archetypes.emplace(archetype);
    }
}

void QueryArchetypes::lock() {
    m_world.defer();
}

void QueryArchetypes::unlock() {
    m_world.deferEnd();
}

QueryArchetypes::Iterator::Iterator() = default;

QueryArchetypes::Iterator::Iterator (std::unordered_set<Archetype*>::const_iterator it) : m_it{it} {}

QueryArchetypes::Iterator::reference QueryArchetypes::Iterator::operator* () const {
    return **m_it;
}

QueryArchetypes::Iterator& QueryArchetypes::Iterator::operator++ () {
    ++m_it;
    return *this;
}

QueryArchetypes::Iterator QueryArchetypes::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

bool QueryArchetypes::Iterator::operator== (const Iterator& other) const noexcept {
    return m_it == other.m_it;
}
