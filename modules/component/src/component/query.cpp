#include "component/query.h"

#include "component/component.h"

using namespace phenyl::component;

QueryArchetypes::QueryArchetypes (World& world, std::vector<std::size_t> componentIds) : world{world}, componentIds{std::move(componentIds)} {}

void QueryArchetypes::onNewArchetype (Archetype* archetype) {
    auto it = componentIds.begin();
    auto archIt = archetype->getComponentIds().begin();

    while (it != componentIds.end() && archIt != archetype->getComponentIds().end()) {
        if (*it < *archIt) {
            // Component doesnt exist in archetype
            return;
        } else if (*it > *archIt) {
            // Extra component in archetype
            ++archIt;
        } else {
            ++it;
            ++archIt;
        }
    }

    if (it == componentIds.end()) {
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

/*QueryArchetypes::Iterator& QueryArchetypes::Iterator::operator-- () {
    --it;
    return *this;
}

QueryArchetypes::Iterator QueryArchetypes::Iterator::operator-- (int) {
    auto copy = *this;
    --*this;
    return copy;
}*/

bool QueryArchetypes::Iterator::operator== (const Iterator& other) const noexcept {
    return it == other.it;
}
