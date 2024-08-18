#include "component/archetype.h"
#include "component/component.h"

using namespace phenyl::component;

Archetype::Archetype(detail::IArchetypeManager& manager, std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> components) : manager{manager}, key{components | std::ranges::views::keys}, components{std::move(components)} {}

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

void Archetype::clear() {
    for (auto& [_, vec] : components) {
        vec->clear();
    }
    entityIds.clear();
}

void Archetype::instantiatePrefab (const detail::PrefabFactories& factories, std::size_t pos) {
    PHENYL_DASSERT(pos < size());

    /*std::vector<std::size_t> compIds;
    compIds.reserve(componentIds.size() + factories.size());
    auto compIt = componentIds.begin();
    auto factIt = factories.begin();
    while (compIt != componentIds.end() && factIt != factories.end()) {
        if (*compIt < factIt->first) {
            compIds.emplace_back(*compIt);
            ++compIt;
        } else if (*compIt > factIt->first) {
            compIds.emplace_back(factIt->first);
            ++factIt;
        } else {
            compIds.emplace_back(*compIt);
            ++compIt;
            ++factIt;
        }
    }

    while (compIt != componentIds.end()) {
        compIds.emplace_back(*(compIt++));
    }

    while (factIt != factories.end()) {
        compIds.emplace_back((factIt++)->first);
    }*/

    auto* archetype = manager.findArchetype(key.with(factories | std::ranges::views::keys));
    PHENYL_DASSERT(archetype);
    auto newPos = archetype->moveFrom(*this, pos);
    remove(pos);
    archetype->instantiateInto(factories, newPos);
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

    manager.updateEntityEntry(entityIds.back(), this, newPos);
    return newPos;
}

void Archetype::instantiateInto (const detail::PrefabFactories& factories, std::size_t pos) {
    PHENYL_DASSERT(pos == size() - 1);
    std::vector<std::size_t> newComps; // TODO

    auto compIt = components.begin();
    auto facIt = factories.begin();
    while (compIt != components.end() && facIt != factories.end()) {
        if (compIt->first < facIt->first) {
            ++compIt;
        } else {
            PHENYL_DASSERT(compIt->first == facIt->first);
            if (compIt->second->size() != size()) {
                auto* ptr = compIt->second->insertUntyped();
                facIt->second->make(ptr);
                newComps.emplace_back(compIt->first);
            }

            ++compIt;
            ++facIt;
        }
    }

    for (auto c : newComps) {
        manager.onComponentInsert(entityIds.back(), c, components[c]->getUntyped(pos));
    }
}

static_assert(std::random_access_iterator<ArchetypeView<int, float>::Iterator>);
