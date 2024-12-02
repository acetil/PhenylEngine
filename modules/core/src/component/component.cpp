#include "core/world.h"
#include "core/signals/children_update.h"
#include "core/detail/loggers.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"MANAGER", phenyl::core::detail::COMPONENT_LOGGER};

World::World (std::size_t capacity) : idList{capacity}, relationships{capacity}, prefabManager{std::make_shared<PrefabManager>(*this)} {
    auto empty = std::make_unique<EmptyArchetype>(static_cast<detail::IArchetypeManager&>(*this));
    emptyArchetype = empty.get();
    archetypes.emplace_back(std::move(empty));
}

World::~World() = default;

Entity World::create (EntityId parent)  {
    auto id = idList.newId();

    if (id.pos() == entityEntries.size()) {
        entityEntries.emplace_back(nullptr, 0);
    }

    if (deferCount) {
        deferredCreations.emplace_back(id, parent);
    } else {
        completeCreation(id, parent);
    }

    return Entity{id, this};
}

void World::remove (EntityId id)  {
    if (!idList.check(id)) {
        PHENYL_LOGE(LOGGER, "Attempted to delete invalid entity {}!", id.value());
        return;
    }

    if (removeDeferCount) {
        deferredRemovals.emplace_back(id);
    } else {
        removeInt(id, true);
    }
}

void World::reparent (EntityId id, EntityId parent)  {
    auto oldParent = relationships.parent(id);
    if (oldParent) {
        entity(oldParent).raise(OnRemoveChild{entity(id)});
    }

    relationships.removeFromParent(id);
    relationships.setParent(id, parent);

    if (parent) {
        entity(parent).raise(OnAddChild{entity(id)});
    }
}

void World::clear() {
    PHENYL_ASSERT(!deferCount);
    PHENYL_ASSERT(!removeDeferCount);

    for (const auto& i : archetypes) {
        i->clear();
    }

    for (auto& entry : entityEntries) {
        entry.archetype = nullptr;
        entry.pos = 0;
    }

    idList.clear();
}

Entity World::entity (EntityId id) noexcept {
    return Entity{id, this};
}

ChildrenView World::root () noexcept {
    return ChildrenView{EntityId{}, this};
}

Entity World::parent (EntityId id) noexcept {
    auto parentId = relationships.parent(id);
    return Entity{parentId, this};
}

void World::defer () {
    if (deferCount++) {
        // Already deferred
        return;
    }

    deferRemove();
    prefabManager->defer();
    deferSignals();
}

void World::deferEnd () {
    if (--deferCount) {
        // Still deferring
        return;
    }

    // Create deferred entities
    for (auto [id, parent] : deferredCreations) {
        completeCreation(id, parent);
    }
    deferredCreations.clear();

    // Insert / Erase deferred components
    for (auto& [_, comp] : components) {
        comp->deferEnd();
    }

    // Do deferred instantiations
    prefabManager->deferEnd();

    // Apply deferred functions
    for (auto& [id, func] : deferredApplys) {
        if (exists(id)) {
            func(entity(id));
        }
    }
    deferredApplys.clear();

    deferSignalsEnd();
    deferRemoveEnd();
}

void World::deferSignals () {
    if (signalDeferCount++) {
        // Already deferred
        return;
    }

    for (auto& [_, vec] : signalHandlerVectors) {
        vec->defer();
    }
}

void World::deferSignalsEnd () {
    if (--signalDeferCount) {
        // Still deferring
        return;
    }

    deferRemove();
    for (auto& [_, vec] : signalHandlerVectors) {
        vec->deferEnd();
    }
    deferRemoveEnd();
}

void World::deferRemove() {
    removeDeferCount++;
}

void World::deferRemoveEnd() {
    if (--removeDeferCount) {
        return;
    }

    // In case removal happens while removing deferred entities
    for (std::size_t i = 0; i < deferredRemovals.size(); i++) {
        if (exists(deferredRemovals[i])) {
            removeInt(deferredRemovals[i], true);
        }
    }
    deferredRemovals.clear();
}

PrefabBuilder World::buildPrefab () {
    return prefabManager->makeBuilder();
}

World::iterator World::begin () {
    return iterator{this, idList.cbegin()};
}

World::iterator World::end () {
    return iterator{this, idList.cend()};
}

void World::completeCreation(EntityId id, EntityId parent) {
    relationships.add(id, parent);

    // Entities start out in empty archetype
    emptyArchetype->add(id);

    if (parent) {
        entity(parent).raise(OnAddChild{entity(id)});
    }
}

void World::removeInt (EntityId id, bool updateParent) {
    if (updateParent) {
        auto parentId = relationships.parent(id);
        if (parentId) {
            entity(parentId).raise(OnRemoveChild{entity(id)});
        }
    }

    // Recursively delete children
    auto curr = relationships.entityChildren(id);
    while (curr) {
        auto next = relationships.next(curr);
        removeInt(curr, false);
        curr = next;
    }

    relationships.remove(id, updateParent);

    // Clear entry
    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    PHENYL_DASSERT(entry.archetype);
    entry.archetype->remove(entry.pos);
    entry.archetype = nullptr;
    entry.pos = 0;

    idList.removeId(id);
}

Archetype* World::findArchetype (const detail::ArchetypeKey& key) {
    auto it = std::ranges::find_if(archetypes, [&] (const auto& arch) {
        return arch->getKey() == key;
    });

    if (it != archetypes.end()) {
        return it->get();
    }

    // Build new archetype

    // Create component vectors
    std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> compVecs;
    for (auto i : key) {
        auto compIt = components.find(i);
        PHENYL_ASSERT_MSG(compIt != components.end(), "Failed to find component in findArchetype()");

        compVecs.emplace(i, compIt->second->makeVector());
    }

    auto archetype = std::make_unique<Archetype>(static_cast<detail::IArchetypeManager&>(*this), std::move(compVecs));
    auto* ptr = archetype.get();
    archetypes.emplace_back(std::move(archetype));

    // Update queries
    cleanupQueryArchetypes();
    for (const auto& i : queryArchetypes) {
        i.lock()->onNewArchetype(ptr);
    }

    return ptr;
}

void World::updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) {
    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    entry.archetype = archetype;
    entry.pos = pos;
}

void World::onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onInsert(id, ptr);
}

void World::onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onRemove(id, ptr);
}

void World::deferInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(deferCount);
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->deferComp(id, ptr);
}

void World::deferErase(EntityId id, std::size_t compType) {
    PHENYL_DASSERT(deferCount);
    PHENYL_DASSERT(components.contains(compType));
    auto& comp = components[compType];
    comp->deferErase(id);
}

void World::deferApply (EntityId id, std::function<void(Entity)> applyFunc) {
    PHENYL_DASSERT(deferCount);
    deferredApplys.emplace_back(id, std::move(applyFunc));
}

void World::instantiatePrefab (EntityId id, const detail::PrefabFactories& factories) {
    PHENYL_DASSERT(exists(id));

    auto& entry = entityEntries[id.pos()];
    entry.archetype->instantiatePrefab(factories, entry.pos);
}

void World::raiseSignal (EntityId id, std::size_t signalType, std::byte* ptr) {
    auto vecIt = signalHandlerVectors.find(signalType);
    if (vecIt == signalHandlerVectors.end()) {
        PHENYL_TRACE(LOGGER, "Ignored signal type {} for entity {} that has no handlers", signalType, id.value());
        return;
    }

    deferRemove();
    vecIt->second->handle(id, ptr);
    deferRemoveEnd();
}

std::shared_ptr<QueryArchetypes> World::makeQueryArchetypes (detail::ArchetypeKey key) {
    cleanupQueryArchetypes();

    for (const auto& weakArch : queryArchetypes) {
        if (auto ptr = weakArch.lock(); ptr->getKey() == key) {
            return ptr;
        }
    }

    auto newArch = std::make_shared<QueryArchetypes>(*this, std::move(key));
    queryArchetypes.emplace_back(newArch);
    return newArch;
}

void World::cleanupQueryArchetypes () {
    std::erase_if(queryArchetypes, [] (const auto& x) {
        return x.expired();
    });
}

World::EntityIterator::value_type World::EntityIterator::operator* () const {
    PHENYL_DASSERT(world);
    return world->entity(*it);
}

World::EntityIterator& World::EntityIterator::operator++ () {
    ++it;
    return *this;
}


World::EntityIterator World::EntityIterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

World::EntityIterator& World::EntityIterator::operator-- () {
    --it;
    return *this;
}

World::EntityIterator World::EntityIterator::operator-- (int) {
    auto copy = *this;
    --*this;
    return copy;
}

bool World::EntityIterator::operator== (const EntityIterator& other) const {
    return it == other.it;
}
