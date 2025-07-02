#include "core/component/archetype.h"

#include "core/world.h"

using namespace phenyl::core;

Archetype::Archetype (detail::IArchetypeManager& manager,
    std::map<meta::TypeIndex, std::unique_ptr<UntypedComponentVector>> components) :
    m_manager{manager},
    m_key{components | std::ranges::views::keys},
    m_components{std::move(components)} {
    for (auto& [type, vec] : m_components) {
        auto* instance = m_manager.findComponent(type);
        PHENYL_DASSERT(instance);

        m_interfaces.emplace(type, vec.get());
    }
}

Archetype::Archetype (detail::IArchetypeManager& manager) : m_manager{manager} {}

std::size_t Archetype::addEntity (EntityId id) {
    auto pos = m_entityIds.size();
    m_entityIds.emplace_back(id);

    m_manager.updateEntityEntry(id, this, pos);
    return pos;
}

void Archetype::remove (std::size_t pos) {
    PHENYL_DASSERT(pos < size());

    for (auto& [_, vec] : m_components) {
        vec->remove(pos);
    }

    if (pos != size() - 1) {
        m_entityIds[pos] = m_entityIds.back();
        m_manager.updateEntityEntry(m_entityIds[pos], this, pos);
    }
    m_entityIds.pop_back();
}

void Archetype::clear () {
    for (auto& [_, vec] : m_components) {
        vec->clear();
    }
    m_entityIds.clear();
}

void Archetype::instantiatePrefab (const detail::PrefabFactories& factories, std::size_t pos) {
    PHENYL_DASSERT(pos < size());

    auto* archetype = m_manager.findArchetype(m_key.with(factories | std::ranges::views::keys));
    PHENYL_DASSERT(archetype);
    auto newPos = archetype->moveFrom(*this, pos);
    remove(pos);
    archetype->instantiateInto(factories, newPos);
}

UntypedComponentVector* Archetype::tryGetVector (meta::TypeIndex type) const {
    auto compIt = m_components.find(type);
    if (compIt != m_components.end()) {
        return compIt->second.get();
    }

    auto it = m_interfaces.find(type);
    return it != m_interfaces.end() ? it->second : nullptr;
}

std::size_t Archetype::moveFrom (Archetype& other, std::size_t pos) {
    auto newPos = addEntity(other.m_entityIds[pos]);

    auto it = m_components.begin();
    auto otherIt = other.m_components.begin();
    while (it != m_components.end() && otherIt != other.m_components.end()) {
        if (it->first < otherIt->first) {
            // Not in other archetype, skip
            ++it;
        } else if (it->first > otherIt->first) {
            // Not in this archetype, skip
            ++otherIt;
        } else {
            it->second->moveFrom(*otherIt->second, pos);
            PHENYL_DASSERT(m_entityIds.size() == it->second->size());

            ++it;
            ++otherIt;
        }
    }

    // manager.updateEntityEntry(entityIds.back(), this, newPos);
    return newPos;
}

void Archetype::instantiateInto (const detail::PrefabFactories& factories, std::size_t pos) {
    PHENYL_DASSERT(pos == size() - 1);
    std::vector<meta::TypeIndex> newComps;

    auto compIt = m_components.begin();
    auto facIt = factories.begin();
    while (compIt != m_components.end() && facIt != factories.end()) {
        if (compIt->first < facIt->first) {
            // Component not in factories, skip
            ++compIt;
        } else {
            PHENYL_DASSERT(compIt->first == facIt->first);
            if (compIt->second->size() != size()) {
                // Component doesnt exist yet, make new component
                auto* ptr = compIt->second->insertUntyped();
                facIt->second->make(ptr);
                newComps.emplace_back(compIt->first);
            }

            ++compIt;
            ++facIt;
        }
    }

    // Raise insert signals for only new components
    for (auto c : newComps) {
        m_manager.onComponentInsert(m_entityIds.back(), c, m_components[c]->getUntyped(pos));
    }
}

static_assert(std::random_access_iterator<ArchetypeView<int, float>::Iterator>);
