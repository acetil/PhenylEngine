#pragma once
#include "assets/asset.h"
#include "core/component/detail/prefab_factory.h"
#include "entity.h"
#include "util/type_index.h"

#include <map>
#include <memory>
#include <unordered_set>

namespace phenyl::core {
class Archetype;
class World;
class PrefabManager;
class PrefabBuilder;

class Prefab : public AssetType2<Prefab> {
public:
    Prefab ();
    ~Prefab () override;

    Prefab (const Prefab& other);
    Prefab (Prefab&& other) noexcept;

    Prefab& operator= (const Prefab&);
    Prefab& operator= (Prefab&& other) noexcept;

    void instantiate (Entity entity) const;

    explicit operator bool () const noexcept {
        return m_id;
    }

private:
    std::size_t m_id;
    std::weak_ptr<PrefabManager> m_manager{};

    friend class PrefabBuilder;
    friend class PrefabManager;
    Prefab (std::size_t prefabId, std::weak_ptr<PrefabManager> manager);
};

struct PrefabEntry {
    detail::PrefabFactories factories;
    std::vector<std::size_t> childEntries;
    std::size_t refCount;
};

class PrefabManager : public std::enable_shared_from_this<PrefabManager> {
public:
    explicit PrefabManager (World& world);

    Prefab makePrefab (detail::PrefabFactories factories, std::vector<std::size_t> children);
    PrefabBuilder makeBuilder ();

    void incrementRefCount (std::size_t prefabId);
    void decrementRefCount (std::size_t prefabId);
    void instantiate (std::size_t prefabId, Entity entity);

    void defer ();
    void deferEnd ();

private:
    World& m_world;
    std::unordered_map<std::size_t, PrefabEntry> m_entries;
    std::size_t m_nextPrefabId = 1;
    std::vector<std::pair<EntityId, std::size_t>> m_deferredInstantiations;
    bool m_deferring = false;
};

class PrefabBuilder {
public:
    template <typename T>
    PrefabBuilder& with (T&& comp) {
        factories.emplace(meta::TypeIndex::Get<T>(),
            std::make_unique<detail::CopyPrefabFactory<T>>(std::forward<T>(comp)));
        return *this;
    }

    template <typename T>
    PrefabBuilder& with (std::function<T()> func) {
        factories.emplace(meta::TypeIndex::Get<T>(), std::make_unique<detail::FuncPrefabFactory<T>>(std::move(func)));
        return *this;
    }

    PrefabBuilder& withChild (const Prefab& child);
    Prefab build ();

private:
    PrefabManager& manager;
    detail::PrefabFactories factories;
    std::vector<std::size_t> children;

    friend PrefabManager;
    explicit PrefabBuilder (PrefabManager& manager);
};
} // namespace phenyl::core
