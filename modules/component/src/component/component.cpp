#include "component/component.h"
#include "component/signals/children_update.h"

using namespace phenyl::component;

static phenyl::Logger LOGGER{"MANAGER", detail::COMPONENT_LOGGER};

ComponentManager::ComponentManager (std::size_t capacity) : idList{capacity}, relationships{capacity}, prefabManager{std::make_shared<PrefabManager>(*this)} {
    auto empty = std::make_unique<EmptyArchetype>(static_cast<detail::IArchetypeManager&>(*this));
    emptyArchetype = empty.get();
    archetypes.emplace_back(std::move(empty));
}

ComponentManager::~ComponentManager() = default;

Entity ComponentManager::create (EntityId parent)  {
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

void ComponentManager::remove (EntityId id)  {
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

void ComponentManager::reparent(EntityId id, EntityId parent)  {
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

void ComponentManager::clear() {
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

Entity ComponentManager::entity (EntityId id) noexcept {
    return Entity{id, this};
}

ChildrenView ComponentManager::root () noexcept {
    return ChildrenView{EntityId{}, this};
}

Entity ComponentManager::parent (EntityId id) noexcept {
    auto parentId = relationships.parent(id);
    return Entity{parentId, this};
}

void ComponentManager::defer () {
    if (deferCount++) {
        // Already deferred
        return;
    }

    deferRemove();
    prefabManager->defer();
    deferSignals();
}

void ComponentManager::deferEnd () {
    if (--deferCount) {
        // Still deferring
        return;
    }

    for (auto [id, parent] : deferredCreations) {
        completeCreation(id, parent);
    }
    deferredCreations.clear();

    for (auto& [_, comp] : components) {
        comp->deferEnd();
    }

    prefabManager->deferEnd();

    for (auto& [id, func] : deferredApplys) {
        if (exists(id)) {
            func(entity(id));
        }
    }
    deferredApplys.clear();

    deferSignalsEnd();
    deferRemoveEnd();
}

void ComponentManager::deferSignals () {
    if (signalDeferCount++) {
        // Already deferred
        return;
    }

    for (auto& [_, vec] : signalHandlerVectors) {
        vec->defer();
    }
}

void ComponentManager::deferSignalsEnd () {
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

void ComponentManager::deferRemove() {
    removeDeferCount++;
}

void ComponentManager::deferRemoveEnd() {
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

PrefabBuilder ComponentManager::buildPrefab () {
    return prefabManager->makeBuilder();
}

void ComponentManager::completeCreation(EntityId id, EntityId parent) {
    relationships.add(id, parent);

    emptyArchetype->add(id);

    if (parent) {
        entity(parent).raise(OnAddChild{entity(id)});
    }
}

void ComponentManager::removeInt (EntityId id, bool updateParent) {
    if (updateParent) {
        auto parentId = relationships.parent(id);
        if (parentId) {
            entity(parentId).raise(OnRemoveChild{entity(id)});
        }
    }

    auto curr = relationships.entityChildren(id);
    while (curr) {
        auto next = relationships.next(curr);
        removeInt(curr, false);
        curr = next;
    }

    relationships.remove(id, updateParent);

    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    PHENYL_DASSERT(entry.archetype);
    entry.archetype->remove(entry.pos);
    entry.archetype = nullptr;
    entry.pos = 0;

    idList.removeId(id);
}

Archetype* ComponentManager::findArchetype (const std::vector<std::size_t>& comps) {
    auto it = std::ranges::find_if(archetypes, [&] (const auto& arch) {
        return arch->getComponentIds() == comps;
    });

    if (it != archetypes.end()) {
        return it->get();
    }

    std::map<std::size_t, std::unique_ptr<UntypedComponentVector>> compVecs;
    for (auto i : comps) {
        auto compIt = components.find(i);
        PHENYL_ASSERT_MSG(compIt != components.end(), "Failed to find component in findArchetype()");

        compVecs.emplace(i, compIt->second->makeVector());
    }

    auto archetype = std::make_unique<Archetype>(static_cast<detail::IArchetypeManager&>(*this), std::move(compVecs));
    auto* ptr = archetype.get();
    archetypes.emplace_back(std::move(archetype));

    cleanupQueryArchetypes();
    for (const auto& i : queryArchetypes) {
        i.lock()->onNewArchetype(ptr);
    }

    return ptr;
}

void ComponentManager::updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) {
    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    entry.archetype = archetype;
    entry.pos = pos;
}

void ComponentManager::onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onInsert(id, ptr);
}

void ComponentManager::onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onRemove(id, ptr);
}

void ComponentManager::deferInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(deferCount);
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->deferComp(id, ptr);
}

void ComponentManager::deferApply (EntityId id, std::function<void(Entity)> applyFunc) {
    PHENYL_DASSERT(deferCount);
    deferredApplys.emplace_back(id, std::move(applyFunc));
}

void ComponentManager::instantiatePrefab (EntityId id, const detail::PrefabFactories& factories) {
    PHENYL_DASSERT(exists(id));

    auto& entry = entityEntries[id.pos()];
    entry.archetype->instantiatePrefab(factories, entry.pos);
}

void ComponentManager::raiseSignal (EntityId id, std::size_t signalType, std::byte* ptr) {
    auto vecIt = signalHandlerVectors.find(signalType);
    if (vecIt == signalHandlerVectors.end()) {
        PHENYL_LOGD(LOGGER, "Ignored signal type {} for entity {} that has no handlers", signalType, id.value());
        return;
    }

    deferRemove();
    vecIt->second->handle(id, ptr);
    deferRemoveEnd();
}

std::shared_ptr<QueryArchetypes> ComponentManager::makeQueryArchetypes (std::vector<std::size_t> components) {
    cleanupQueryArchetypes();
    std::sort(components.begin(), components.end());

    for (const auto& weakArch : queryArchetypes) {
        if (auto ptr = weakArch.lock(); ptr->components() == components) {
            return ptr;
        }
    }

    auto newArch = std::make_shared<QueryArchetypes>(*this, std::move(components));
    queryArchetypes.emplace_back(newArch);
    return newArch;
}

void ComponentManager::cleanupQueryArchetypes () {
    std::erase_if(queryArchetypes, [] (const auto& x) {
        return x.expired();
    });
}
