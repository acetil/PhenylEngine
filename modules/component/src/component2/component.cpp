#include "component2/signals/children_update.h"
#include "component2/component.h"

using namespace phenyl::component;


ComponentManager2::ComponentManager2 (std::size_t capacity) : idList{capacity}, relationships{capacity}, prefabManager{std::make_shared<PrefabManager2>(*this)} {
    auto empty = std::make_unique<EmptyArchetype>(static_cast<detail::IArchetypeManager&>(*this));
    emptyArchetype = empty.get();
    archetypes.emplace_back(std::move(empty));
}

Entity2 ComponentManager2::create (EntityId parent)  {
    auto id = idList.newId();

    if (id.pos() == entityEntries.size()) {
        entityEntries.emplace_back(nullptr, 0);
    }
    relationships.add(id, parent);

    emptyArchetype->add(id);

    if (parent) {
        entity(parent).raise(OnAddChild2{entity(id)});
    }

    return Entity2{id, this};
}

void ComponentManager2::remove (EntityId id)  {
    if (!idList.check(id)) {
        PHENYL_LOGE(detail::MANAGER_LOGGER, "Attempted to delete invalid entity {}!", id.value());
        return;
    }

    // TODO: defer

    removeInt(id, true);
}

void ComponentManager2::reparent(EntityId id, EntityId parent)  {
    auto oldParent = relationships.parent(id);
    if (oldParent) {
        entity(oldParent).raise(OnRemoveChild2{entity(id)});
    }

    relationships.removeFromParent(id);
    relationships.setParent(id, parent);

    if (parent) {
        entity(parent).raise(OnAddChild2{entity(id)});
    }
}

void ComponentManager2::clear() {
    for (const auto& i : archetypes) {
        i->clear();
    }
}

Entity2 ComponentManager2::entity (EntityId id) noexcept {
    return Entity2{id, this};
}

ChildrenView2 ComponentManager2::root () noexcept {
    return ChildrenView2{EntityId{}, this};
}

Entity2 ComponentManager2::parent (EntityId id) noexcept {
    auto parentId = relationships.parent(id);
    return Entity2{parentId, this};
}

PrefabBuilder2 ComponentManager2::buildPrefab () {
    return prefabManager->makeBuilder();
}

void ComponentManager2::removeInt (EntityId id, bool updateParent) {
    if (updateParent) {
        auto parentId = relationships.parent(id);
        if (parentId) {
            entity(parentId).raise(OnRemoveChild2{entity(id)});
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

Archetype* ComponentManager2::findArchetype (const std::vector<std::size_t>& comps) {
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

void ComponentManager2::updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) {
    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    entry.archetype = archetype;
    entry.pos = pos;
}

void ComponentManager2::onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onInsert(id, ptr);
}

void ComponentManager2::onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) {
    PHENYL_DASSERT(components.contains(compType));

    auto& comp = components[compType];
    comp->onRemove(id, ptr);
}

Entity2 ComponentManager2::makeWithPrefab (EntityId parent, Archetype* archetype, std::size_t prefabId) {
    auto id = idList.newId();

    if (id.pos() == entityEntries.size()) {
        entityEntries.emplace_back(nullptr, 0);
    }
    relationships.add(id, parent);

    archetype->addWithPrefab(id, prefabId);

    if (parent) {
        entity(parent).raise(OnAddChild2{entity(id)});
    }

    return entity(id);
}


void ComponentManager2::raiseSignal (Entity2 entity, std::size_t signalType, const std::byte* ptr) {
    auto vecIt = signalHandlerVectors.find(signalType);
    if (vecIt == signalHandlerVectors.end()) {
        return;
    }

    vecIt->second->handle(entity, ptr);
}

std::shared_ptr<QueryArchetypes> ComponentManager2::makeQueryArchetypes (std::vector<std::size_t> components) {
    cleanupQueryArchetypes();

    for (const auto& weakArch : queryArchetypes) {
        if (auto ptr = weakArch.lock(); ptr->components() == components) {
            return ptr;
        }
    }

    auto newArch = std::make_shared<QueryArchetypes>(std::move(components));
    queryArchetypes.emplace_back(newArch);
    return newArch;
}

void ComponentManager2::cleanupQueryArchetypes () {
    std::erase_if(queryArchetypes, [] (const auto& x) {
        return x.expired();
    });
}
