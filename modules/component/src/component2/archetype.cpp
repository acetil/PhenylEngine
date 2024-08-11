#include "component2/archetype.h"

using namespace phenyl::component;

Archetype::Archetype (const Archetype& other, std::unique_ptr<UntypedComponentVector> compVec) : manager{other.manager} {
    PHENYL_DASSERT(!other.hasUntyped(compVec->type()));

    for (const auto& [type, vec] : other.components) {
        components.emplace(type, vec->makeNew());
        componentIds.emplace_back(type);
    }

    auto vecType = compVec->type();
    components.emplace(vecType, std::move(compVec));

    componentIds.reserve(other.components.size() + 1);
    std::transform(components.begin(), components.end(), std::back_inserter(componentIds), [] (const auto& p) {
        return p.first;
    });
}

Archetype::Archetype (detail::IArchetypeManager& manager) : manager{manager} {}

void Archetype::addEntity(EntityId id) {
    auto pos = entityIds.size();
    entityIds.emplace_back(id);

    manager.updateEntityEntry(id, this, pos);
}

void Archetype::remove (std::size_t pos) {
    PHENYL_DASSERT(pos < size());

    for (auto& [_, vec] : components) {
        vec->remove(pos);
    }

    if (pos != size() - 1) {
        entityIds[pos] = entityIds.back();
        manager.updateEntityEntry(entityIds[pos], this, pos);
    }
    entityIds.pop_back();
}

std::size_t Archetype::moveFrom (Archetype& other, std::size_t pos) {
    auto newPos = entityIds.size();
    entityIds.emplace_back(other.entityIds[pos]);

    auto it = components.begin();
    auto otherIt = other.components.begin();
    while (it != components.end() && otherIt != other.components.end()) {
        if (it->first < otherIt->first) {
            ++it;
        } else if (it->first > otherIt->first) {
            ++otherIt;
        } else {
            it->second->moveFrom(*otherIt->second, pos);
            PHENYL_DASSERT(entityIds.size() == it->second->size());

            ++it;
            ++otherIt;
        }
    }

    return newPos;
}

static_assert(std::random_access_iterator<ArchetypeView<int, float>::Iterator>);