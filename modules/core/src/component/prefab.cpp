#include "core/prefab.h"

#include "core/detail/loggers.h"
#include "core/world.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"PREFAB", phenyl::core::detail::COMPONENT_LOGGER};

std::string_view Prefab::GetAssetType () {
    return "phenyl::Prefab";
}

Prefab::Prefab () : m_id{0} {}

Prefab::Prefab (std::size_t prefabId, std::weak_ptr<PrefabManager> manager) :
    m_id{prefabId},
    m_manager{std::move(manager)} {
    PHENYL_DASSERT(this->m_manager.lock());
}

Prefab::~Prefab () {
    if (m_id) {
        if (const auto ptr = m_manager.lock(); ptr) {
            ptr->decrementRefCount(m_id);
        }
    }
}

Prefab::Prefab (const Prefab& other) : m_id{other.m_id}, m_manager{other.m_manager} {
    if (other.m_id) {
        if (const auto ptr = m_manager.lock(); ptr) {
            ptr->incrementRefCount(other.m_id);
        }
    }
}

Prefab::Prefab (Prefab&& other) noexcept : m_id{other.m_id}, m_manager{std::move(other.m_manager)} {
    other.m_id = 0;
}

Prefab& Prefab::operator= (const Prefab& other) {
    if (&other == this) {
        return *this;
    }

    if (m_id) {
        if (const auto ptr = m_manager.lock(); ptr) {
            ptr->decrementRefCount(m_id);
        }
    }

    m_id = other.m_id;
    m_manager = other.m_manager;

    if (other.m_id) {
        if (const auto ptr = m_manager.lock(); ptr) {
            ptr->incrementRefCount(other.m_id);
        }
    }

    return *this;
}

Prefab& Prefab::operator= (Prefab&& other) noexcept {
    if (m_id) {
        if (const auto ptr = m_manager.lock(); ptr) {
            ptr->decrementRefCount(m_id);
        }
    }

    m_id = other.m_id;
    m_manager = std::move(other.m_manager);

    other.m_id = 0;

    return *this;
}

void Prefab::instantiate (Entity entity) const {
    auto ptr = m_manager.lock();
    PHENYL_ASSERT_MSG(ptr, "Attempted to create entity with prefab from already deleted PrefabManager!");

    ptr->instantiate(m_id, entity);
}

PrefabManager::PrefabManager (World& world) : m_world{world} {}

Prefab PrefabManager::makePrefab (detail::PrefabFactories factories, std::vector<std::size_t> children) {
    for (auto i : children) {
        PHENYL_DASSERT(m_entries.contains(i));
    }

    auto id = m_nextPrefabId++;
    m_entries.emplace(id,
        PrefabEntry{
          .factories = std::move(factories),
          .childEntries = std::move(children),
          .refCount = 1,
        });

    return Prefab{id, weak_from_this()};
}

PrefabBuilder PrefabManager::makeBuilder () {
    return PrefabBuilder{*this};
}

void PrefabManager::incrementRefCount (std::size_t prefabId) {
    PHENYL_DASSERT(m_entries.contains(prefabId));
    m_entries[prefabId].refCount++;
}

void PrefabManager::decrementRefCount (std::size_t prefabId) {
    auto entryIt = m_entries.find(prefabId);
    PHENYL_DASSERT(entryIt != m_entries.end());

    auto& entry = entryIt->second;
    if (!(--entry.refCount)) {
        // Delete prefab
        for (auto i : entry.childEntries) {
            decrementRefCount(i);
        }

        m_entries.erase(entryIt);
    }
}

void PrefabManager::instantiate (std::size_t prefabId, Entity entity) {
    PHENYL_DASSERT(m_entries.contains(prefabId));

    if (m_deferring) {
        m_deferredInstantiations.emplace_back(entity.id(), prefabId);
        incrementRefCount(prefabId); // So prefab doesnt get deleted while deferring
        return;
    }

    const auto& entry = m_entries[prefabId];
    m_world.instantiatePrefab(entity.id(), entry.factories);

    for (auto i : entry.childEntries) {
        instantiate(i, m_world.create(entity.id()));
    }
}

void PrefabManager::defer () {
    PHENYL_DASSERT(!m_deferring);
    PHENYL_DASSERT(m_deferredInstantiations.empty());

    m_deferring = true;
}

void PrefabManager::deferEnd () {
    PHENYL_DASSERT(m_deferring);
    m_deferring = false;

    for (auto [id, prefabId] : m_deferredInstantiations) {
        Entity entity = m_world.entity(id);

        if (entity.exists()) {
            instantiate(prefabId, entity);
            decrementRefCount(prefabId);
        }
    }
    m_deferredInstantiations.clear();
}

PrefabBuilder::PrefabBuilder (PrefabManager& manager) : manager{manager} {}

PrefabBuilder& PrefabBuilder::withChild (const Prefab& child) {
    if (child) {
        manager.incrementRefCount(child.m_id);
        children.emplace_back(child.m_id);
    } else {
        PHENYL_LOGE(LOGGER, "Attempted to add invalid prefab as child!");
    }

    return *this;
}

Prefab PrefabBuilder::build () {
    return manager.makePrefab(std::move(factories), std::move(children));
}
