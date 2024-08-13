#include "component2/prefab.h"

#include "component2/component.h"

using namespace phenyl::component;

Prefab2::Prefab2 () : prefabId{0} {}
Prefab2::Prefab2 (std::size_t prefabId, std::weak_ptr<PrefabManager2> manager) : prefabId{prefabId}, manager{std::move(manager)} {
    PHENYL_DASSERT(manager.lock());
}

Prefab2::~Prefab2 () {
    if (prefabId) {
        if (const auto ptr = manager.lock(); ptr) {
            ptr->decrementRefCount(prefabId);
        }
    }
}

Prefab2::Prefab2 (const Prefab2& other) : prefabId{other.prefabId}, manager{other.manager} {
    if (other.prefabId) {
        if (const auto ptr = manager.lock(); ptr) {
            ptr->incrementRefCount(other.prefabId);
        }
    }
}

Prefab2::Prefab2 (Prefab2&& other) noexcept : prefabId{other.prefabId}, manager{std::move(other.manager)} {
    other.prefabId = 0;
}

Prefab2& Prefab2::operator= (const Prefab2& other) {
    if (&other == this) {
        return *this;
    }

    if (prefabId) {
        if (const auto ptr = manager.lock(); ptr) {
            ptr->decrementRefCount(prefabId);
        }
    }

    prefabId = other.prefabId;
    manager = other.manager;

    if (other.prefabId) {
        if (const auto ptr = manager.lock(); ptr) {
            ptr->incrementRefCount(other.prefabId);
        }
    }

    return *this;
}

Prefab2& Prefab2::operator= (Prefab2&& other) noexcept {
    if (prefabId) {
        if (const auto ptr = manager.lock(); ptr) {
            ptr->decrementRefCount(prefabId);
        }
    }

    prefabId = other.prefabId;
    manager = std::move(other.manager);

    other.prefabId = 0;

    return *this;
}

Entity2 Prefab2::create (Entity2 parent) const {
    auto ptr = manager.lock();
    PHENYL_ASSERT_MSG(ptr, "Attempted to create entity with prefab from already deleted PrefabManager!");

    return ptr->instantiate(prefabId, parent);
}

PrefabManager2::PrefabManager2 (ComponentManager2& manager) : manager{manager} {}

Prefab2 PrefabManager2::makePrefab (std::map<std::size_t, std::unique_ptr<detail::IPrefabFactory>> factories, std::vector<std::size_t> children) {
    for (auto i : children) {
        PHENYL_DASSERT(entries.contains(i));
    }

    std::vector<std::size_t> compIds(factories.size());
    std::transform(factories.begin(), factories.end(), compIds, [] (const auto& x) { return x.first; });
    auto* archetype = manager.findArchetype(compIds);
    auto prefabIndex = archetype->addArchetypePrefab(std::move(factories));

    auto id = nextPrefabId++;
    entries.emplace(id, PrefabEntry{
        .archetype = archetype,
        .archPrefabId = prefabIndex,
        .refCount = 1,
        .childEntries = std::move(children)
    });

    return Prefab2{id, weak_from_this()};
}

PrefabBuilder2 PrefabManager2::makeBuilder () {
    return PrefabBuilder2{*this};
}

void PrefabManager2::incrementRefCount (std::size_t prefabId) {
    PHENYL_DASSERT(entries.contains(prefabId));
    entries[prefabId].refCount++;
}

void PrefabManager2::decrementRefCount (std::size_t prefabId) {
    auto entryIt = entries.find(prefabId);
    PHENYL_DASSERT(entryIt != entries.end());

    auto& entry = entryIt->second;
    if (!(--entry.refCount)) {
        entry.archetype->removePrefab(entry.archPrefabId);

        for (auto i : entry.childEntries) {
            decrementRefCount(i);
        }

        entries.erase(entryIt);
    }
}

Entity2 PrefabManager2::instantiate (std::size_t prefabId, Entity2 parent) {
    PHENYL_DASSERT(entries.contains(prefabId));

    const auto& entry = entries[prefabId];
    return manager.makeWithPrefab(parent.id(), entry.archetype, entry.archPrefabId);
}

PrefabBuilder2::PrefabBuilder2 (PrefabManager2& manager) : manager{manager} {}

PrefabBuilder2& PrefabBuilder2::withChild (const Prefab2& child) {
    if (child) {
        manager.incrementRefCount(child.prefabId);
        children.emplace_back(child.prefabId);
    }

    return *this;
}

Prefab2 PrefabBuilder2::build () {
    return manager.makePrefab(std::move(factories), std::move(children));
}
