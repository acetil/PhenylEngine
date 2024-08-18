#pragma once
#include <map>
#include <memory>
#include <unordered_set>

#include "util/meta.h"

#include "entity.h"
#include "detail/prefab_factory.h"

namespace phenyl::component {
    class Archetype;
    class World;
    class PrefabManager;
    class PrefabBuilder;

    class Prefab {
    private:
        std::size_t prefabId;
        std::weak_ptr<PrefabManager> manager{};

        friend class PrefabBuilder;
        friend class PrefabManager;
        Prefab (std::size_t prefabId, std::weak_ptr<PrefabManager> manager);
    public:
        Prefab ();
        ~Prefab();

        Prefab (const Prefab& other);
        Prefab (Prefab&& other) noexcept;

        Prefab& operator= (const Prefab&);
        Prefab& operator= (Prefab&& other) noexcept;

        void instantiate (Entity entity) const;

        explicit operator bool () const noexcept {
            return prefabId;
        }
    };

    struct PrefabEntry {
        detail::PrefabFactories factories;
        std::vector<std::size_t> childEntries;
        std::size_t refCount;
    };

    class PrefabManager : public std::enable_shared_from_this<PrefabManager> {
    private:
        World& world;
        std::unordered_map<std::size_t, PrefabEntry> entries;
        std::size_t nextPrefabId = 1;
        std::vector<std::pair<EntityId, std::size_t>> deferredInstantiations;
        bool deferring = false;

    public:
        explicit PrefabManager (World& world);

        Prefab makePrefab (detail::PrefabFactories factories, std::vector<std::size_t> children);
        PrefabBuilder makeBuilder ();

        void incrementRefCount (std::size_t prefabId);
        void decrementRefCount (std::size_t prefabId);
        void instantiate (std::size_t prefabId, Entity entity);

        void defer ();
        void deferEnd ();
    };

    class PrefabBuilder {
    private:
        PrefabManager& manager;
        detail::PrefabFactories factories;
        std::vector<std::size_t> children;

        friend PrefabManager;
        explicit PrefabBuilder (PrefabManager& manager);
    public:
        template <typename T>
        PrefabBuilder& with (T&& comp) {
            factories.emplace(meta::type_index<T>(), std::make_unique<detail::CopyPrefabFactory<T>>(std::forward<T>(comp)));
            return *this;
        }

        template <typename T>
        PrefabBuilder& with (std::function<T()> func) {
            factories.emplace(meta::type_index<T>(), std::make_unique<detail::FuncPrefabFactory<T>>(std::move(func)));
            return *this;
        }

        PrefabBuilder& withChild (const Prefab& child);
        Prefab build ();
    };
}
