#include "core/component/query.h"

#include "core/world.h"

using namespace phenyl::core;

QueryArchetypes::QueryArchetypes (World& world, detail::ArchetypeKey key) : world{world}, key{std::move(key)} {}

void QueryArchetypes::onNewArchetype (Archetype* archetype) {
    if (archetype->getKey().subsetOf(key)) {
        // All components found
        archetypes.emplace(archetype);
    }
}

void QueryArchetypes::lock() {
    world.defer();
}

void QueryArchetypes::unlock() {
    world.deferEnd();
}

QueryArchetypes::Iterator::Iterator() = default;

QueryArchetypes::Iterator::Iterator (std::unordered_set<Archetype*>::const_iterator it) : it{it} {}

QueryArchetypes::Iterator::reference QueryArchetypes::Iterator::operator* () const {
    return **it;
}

QueryArchetypes::Iterator& QueryArchetypes::Iterator::operator++ () {
    ++it;
    return *this;
}

QueryArchetypes::Iterator QueryArchetypes::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

bool QueryArchetypes::Iterator::operator== (const Iterator& other) const noexcept {
    return it == other.it;
}
