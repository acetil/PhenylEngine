#include "component2/component.h"

using namespace phenyl::component;


ComponentManager2::ComponentManager2 (std::size_t capacity) : idList{capacity}, relationships{capacity} {
    auto empty = std::make_unique<EmptyArchetype>(static_cast<detail::IArchetypeManager&>(*this));
    emptyArchetype = empty.get();
    archetypes.emplace_back(std::move(empty));
}

Entity2 ComponentManager2::create (EntityId parent)  {
    auto id = idList.newId();

    if (id.pos() == entityEntries.size()) {
        entityEntries.emplace_back(nullptr, 0);
    }
    emptyArchetype->add(id);

    relationships.add(id, parent);

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
    // TODO: signals
    relationships.removeFromParent(id);
    relationships.setParent(id, parent);
}

void ComponentManager2::clear() {
    for (const auto& i : archetypes) {
        i->clear();
    }
}

Entity2 ComponentManager2::parent (EntityId id) noexcept {
    auto parentId = relationships.parent(id);
    return Entity2{parentId, this};
}

void ComponentManager2::removeInt (EntityId id, bool updateParent) {
    if (updateParent) {
        // TODO: signal remove child
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

void ComponentManager2::addArchetype (std::unique_ptr<Archetype> archetype) {
    auto* ptr = archetype.get();
    archetypes.emplace_back(std::move(archetype));

    cleanupQueryArchetypes();
    for (const auto& i : queryArchetypes) {
        i.lock()->onNewArchetype(ptr);
    }
}

Archetype* ComponentManager2::findArchetype (const std::vector<std::size_t>& comps) {
    auto it = std::ranges::find_if(archetypes, [&] (const auto& arch) {
        return arch->getComponentIds() == comps;
    });

    return it != archetypes.end() ? it->get() : nullptr;
}

void ComponentManager2::updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) {
    PHENYL_DASSERT(id.pos() < entityEntries.size());
    auto& entry = entityEntries[id.pos()];
    entry.archetype = archetype;
    entry.pos = pos;
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
