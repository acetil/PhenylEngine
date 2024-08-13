#pragma once
#include <map>
#include <memory>
#include <unordered_set>

#include "util/meta.h"

#include "entity2.h"
#include "detail/prefab_factory.h"

namespace phenyl::component {
    class Archetype;
    class ComponentManager2;
    class PrefabManager2;

    class Prefab2 {
    private:
        std::size_t prefabId;
        std::weak_ptr<PrefabManager2> manager{};

        friend class PrefabBuilder2;
        friend class PrefabManager2;
        Prefab2 (std::size_t prefabId, std::weak_ptr<PrefabManager2> manager);
    public:
        Prefab2 ();
        ~Prefab2();

        Prefab2 (const Prefab2& other);
        Prefab2 (Prefab2&& other) noexcept;

        Prefab2& operator= (const Prefab2&);
        Prefab2& operator= (Prefab2&& other) noexcept;

        [[nodiscard]] Entity2 create (Entity2 parent = Entity2{}) const;

        explicit operator bool () const noexcept {
            return prefabId;
        }
    };

    struct PrefabEntry {
        Archetype* archetype;
        std::size_t archPrefabId;
        std::vector<std::size_t> childEntries;
        std::size_t refCount;
    };

    class PrefabManager2 : public std::enable_shared_from_this<PrefabManager2> {
    private:
        ComponentManager2& manager;
        std::unordered_map<std::size_t, PrefabEntry> entries;
        std::size_t nextPrefabId = 1;

    public:
        explicit PrefabManager2 (ComponentManager2& manager);

        Prefab2 makePrefab (std::map<std::size_t, std::unique_ptr<detail::IPrefabFactory>> factories, std::vector<std::size_t> children);
        PrefabBuilder2 makeBuilder ();

        void incrementRefCount (std::size_t prefabId);
        void decrementRefCount (std::size_t prefabId);
        Entity2 instantiate (std::size_t prefabId, Entity2 parent);
    };

    class PrefabBuilder2 {
    private:
        PrefabManager2& manager;
        std::map<std::size_t, std::unique_ptr<detail::IPrefabFactory>> factories;
        std::vector<std::size_t> children;

        friend PrefabManager2;
        explicit PrefabBuilder2 (PrefabManager2& manager);
    public:
        template <typename T>
        PrefabBuilder2& with (T&& comp) {
            factories.emplace(meta::type_index<T>(), std::make_unique<detail::CopyPrefabFactory<T>>(std::forward<T>(comp)));
            return *this;
        }

        template <typename T>
        PrefabBuilder2& with (std::function<T()> func) {
            factories.emplace(meta::type_index<T>(), std::make_unique<detail::FuncPrefabFactory<T>>(std::move(func)));
            return *this;
        }

        PrefabBuilder2& withChild (const Prefab2& child);
        Prefab2 build ();
    };
}
