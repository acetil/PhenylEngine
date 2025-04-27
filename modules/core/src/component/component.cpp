#include "core/world.h"
#include "core/signals/children_update.h"
#include "core/detail/loggers.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"MANAGER", phenyl::core::detail::COMPONENT_LOGGER};

World::World (std::size_t capacity) : m_idList{capacity}, m_relationships{capacity}, m_prefabManager{std::make_shared<PrefabManager>(*this)} {
    auto empty = std::make_unique<EmptyArchetype>(static_cast<detail::IArchetypeManager&>(*this));
    m_emptyArchetype = empty.get();
    m_archetypes.emplace_back(std::move(empty));
}

World::~World() = default;

Entity World::create (EntityId parent)  {
    auto id = m_idList.newId();

    if (id.pos() == m_entityEntries.size()) {
        m_entityEntries.emplace_back(nullptr, 0);
    }

    if (m_deferCount) {
        m_deferredCreations.emplace_back(id, parent);
    } else {
        completeCreation(id, parent);
    }

    return Entity{id, this};
}

void World::remove (EntityId id)  {
    if (!m_idList.check(id)) {
        PHENYL_LOGE(LOGGER, "Attempted to delete invalid entity {}!", id.value());
        return;
    }

    if (m_removeDeferCount) {
        m_deferredRemovals.emplace_back(id);
    } else {
        removeInt(id, true);
    }
}

void World::reparent (EntityId id, EntityId parent)  {
    auto oldParent = m_relationships.parent(id);
    if (oldParent) {
        entity(oldParent).raise(OnRemoveChild{entity(id)});
    }

    m_relationships.removeFromParent(id);
    m_relationships.setParent(id, parent);

    if (parent) {
        entity(parent).raise(OnAddChild{entity(id)});
    }
}

void World::clear() {
    PHENYL_ASSERT(!m_deferCount);
    PHENYL_ASSERT(!m_removeDeferCount);

    for (const auto& i : m_archetypes) {
        i->clear();
    }

    for (auto& entry : m_entityEntries) {
        entry.archetype = nullptr;
        entry.pos = 0;
    }

    m_idList.clear();
}

Entity World::entity (EntityId id) noexcept {
    return Entity{id, this};
}

ChildrenView World::root () noexcept {
    return ChildrenView{EntityId{}, this};
}

Entity World::parent (EntityId id) noexcept {
    auto parentId = m_relationships.parent(id);
    return Entity{parentId, this};
}

void World::defer () {
    if (m_deferCount++) {
        // Already deferred
        return;
    }

    deferRemove();
    m_prefabManager->defer();
    deferSignals();
}

void World::deferEnd () {
    if (--m_deferCount) {
        // Still deferring
        return;
    }

    // Create deferred entities
    for (auto [id, parent] : m_deferredCreations) {
        completeCreation(id, parent);
    }
    m_deferredCreations.clear();

    // Insert / Erase deferred components
    for (auto& [_, comp] : m_components) {
        comp->deferEnd();
    }

    // Do deferred instantiations
    m_prefabManager->deferEnd();

    // Apply deferred functions
    for (auto& [id, func] : m_deferredApplys) {
        if (exists(id)) {
            func(entity(id));
        }
    }
    m_deferredApplys.clear();

    deferSignalsEnd();
    deferRemoveEnd();
}

void World::deferSignals () {
    if (m_signalDeferCount++) {
        // Already deferred
        return;
    }

    for (auto& [_, vec] : m_signalHandlerVectors) {
        vec->defer();
    }
}

void World::deferSignalsEnd () {
    if (--m_signalDeferCount) {
        // Still deferring
        return;
    }

    deferRemove();
    for (auto& [_, vec] : m_signalHandlerVectors) {
        vec->deferEnd();
    }
    deferRemoveEnd();
}

void World::deferRemove() {
    m_removeDeferCount++;
}

void World::deferRemoveEnd() {
    if (--m_removeDeferCount) {
        return;
    }

    // In case removal happens while removing deferred entities
    for (std::size_t i = 0; i < m_deferredRemovals.size(); i++) {
        if (exists(m_deferredRemovals[i])) {
            removeInt(m_deferredRemovals[i], true);
        }
    }
    m_deferredRemovals.clear();
}

PrefabBuilder World::buildPrefab () {
    return m_prefabManager->makeBuilder();
}

World::iterator World::begin () {
    return iterator{this, m_idList.cbegin()};
}

World::iterator World::end () {
    return iterator{this, m_idList.cend()};
}

void World::completeCreation(EntityId id, EntityId parent) {
    m_relationships.add(id, parent);

    // Entities start out in empty archetype
    m_emptyArchetype->add(id);

    if (parent) {
        entity(parent).raise(OnAddChild{entity(id)});
    }
}

void World::removeInt (EntityId id, bool updateParent) {
    if (updateParent) {
        auto parentId = m_relationships.parent(id);
        if (parentId) {
            entity(parentId).raise(OnRemoveChild{entity(id)});
        }
    }

    // Recursively delete children
    auto curr = m_relationships.entityChildren(id);
    while (curr) {
        auto next = m_relationships.next(curr);
        removeInt(curr, false);
        curr = next;
    }

    m_relationships.remove(id, updateParent);

    // Clear entry
    PHENYL_DASSERT(id.pos() < m_entityEntries.size());
    auto& entry = m_entityEntries[id.pos()];
    PHENYL_DASSERT(entry.archetype);
    entry.archetype->remove(entry.pos);
    entry.archetype = nullptr;
    entry.pos = 0;

    m_idList.removeId(id);
}

Archetype* World::findArchetype (const detail::ArchetypeKey& key) {
    auto it = std::ranges::find_if(m_archetypes, [&] (const auto& arch) {
        return arch->getKey() == key;
    });

    if (it != m_archetypes.end()) {
        return it->get();
    }

    // Build new archetype

    // Create component vectors
    std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> compVecs;
    for (auto i : key) {
        auto compIt = m_components.find(i);
        PHENYL_ASSERT_MSG(compIt != m_components.end(), "Failed to find component in findArchetype()");

        compVecs.emplace(i, compIt->second->makeVector());
    }

    auto archetype = std::make_unique<Archetype>(static_cast<detail::IArchetypeManager&>(*this), std::move(compVecs));
    auto* ptr = archetype.get();
    m_archetypes.emplace_back(std::move(archetype));

    // Update queries
    cleanupQueryArchetypes();
    for (const auto& i : m_queryArchetypes) {
        i.lock()->onNewArchetype(ptr);
    }

    return ptr;
}

void World::updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) {
    PHENYL_DASSERT(id.pos() < m_entityEntries.size());
    auto& entry = m_entityEntries[id.pos()];
    entry.archetype = archetype;
    entry.pos = pos;
}

void World::onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(m_components.contains(compType));

    auto& comp = m_components[compType];
    comp->onInsert(id, ptr);
}

void World::onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(m_components.contains(compType));

    auto& comp = m_components[compType];
    comp->onRemove(id, ptr);
}

void World::deferInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(m_deferCount);
    PHENYL_DASSERT(m_components.contains(compType));

    auto& comp = m_components[compType];
    comp->deferComp(id, ptr);
}

void World::deferErase(EntityId id, std::size_t compType) {
    PHENYL_DASSERT(m_deferCount);
    PHENYL_DASSERT(m_components.contains(compType));
    auto& comp = m_components[compType];
    comp->deferErase(id);
}

void World::deferApply (EntityId id, std::function<void(Entity)> applyFunc) {
    PHENYL_DASSERT(m_deferCount);
    m_deferredApplys.emplace_back(id, std::move(applyFunc));
}

void World::instantiatePrefab (EntityId id, const detail::PrefabFactories& factories) {
    PHENYL_DASSERT(exists(id));

    auto& entry = m_entityEntries[id.pos()];
    entry.archetype->instantiatePrefab(factories, entry.pos);
}

void World::raiseSignal (EntityId id, std::size_t signalType, std::byte* ptr) {
    auto vecIt = m_signalHandlerVectors.find(signalType);
    if (vecIt == m_signalHandlerVectors.end()) {
        PHENYL_TRACE(LOGGER, "Ignored signal type {} for entity {} that has no handlers", signalType, id.value());
        return;
    }

    deferRemove();
    vecIt->second->handle(id, ptr);
    deferRemoveEnd();
}

std::shared_ptr<QueryArchetypes> World::makeQueryArchetypes (detail::ArchetypeKey key) {
    cleanupQueryArchetypes();

    for (const auto& weakArch : m_queryArchetypes) {
        if (auto ptr = weakArch.lock(); ptr->getKey() == key) {
            return ptr;
        }
    }

    auto newArch = std::make_shared<QueryArchetypes>(*this, std::move(key));
    m_queryArchetypes.emplace_back(newArch);
    return newArch;
}

void World::cleanupQueryArchetypes () {
    std::erase_if(m_queryArchetypes, [] (const auto& x) {
        return x.expired();
    });
}

World::EntityIterator::value_type World::EntityIterator::operator* () const {
    PHENYL_DASSERT(m_world);
    return m_world->entity(*m_it);
}

World::EntityIterator& World::EntityIterator::operator++ () {
    ++m_it;
    return *this;
}


World::EntityIterator World::EntityIterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

World::EntityIterator& World::EntityIterator::operator-- () {
    --m_it;
    return *this;
}

World::EntityIterator World::EntityIterator::operator-- (int) {
    auto copy = *this;
    --*this;
    return copy;
}

bool World::EntityIterator::operator== (const EntityIterator& other) const {
    return m_it == other.m_it;
}
