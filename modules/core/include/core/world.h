#pragma once

#include "component/archetype.h"
#include "component/children_view.h"
#include "component/detail/component_instance.h"
#include "component/detail/entity_id_list.h"
#include "component/detail/relationships.h"
#include "component/detail/signal_handler.h"
#include "component/forward.h"
#include "component/query.h"
#include "entity.h"
#include "entity_id.h"
#include "prefab.h"

namespace phenyl::core {
class World : private detail::IArchetypeManager {
public:
    class EntityIterator {
    public:
        using value_type = Entity;

        EntityIterator () = default;

        explicit EntityIterator (World* world, detail::EntityIdList::const_iterator it) : m_world{world}, m_it{it} {}

        value_type operator* () const;

        EntityIterator& operator++ ();
        EntityIterator operator++ (int);

        EntityIterator& operator-- ();
        EntityIterator operator-- (int);

        bool operator== (const EntityIterator&) const;

    private:
        World* m_world = nullptr;
        ;
        detail::EntityIdList::const_iterator m_it;
    };

    using iterator = EntityIterator;

    static constexpr std::size_t DEFAULT_CAPACITY = 256;

    explicit World (std::size_t capacity = DEFAULT_CAPACITY);
    ~World () override;

    World (const World&) = delete;
    World (World&& other) = default;

    World& operator= (const World&) = delete;
    World& operator= (World&&) = default;

    template <typename T>
    void addComponent (std::string name) {
        PHENYL_ASSERT_MSG(!m_components.contains(meta::TypeIndex::Get<T>()), "Attempted to add component \"{}\" twice",
            name);

        auto comp = std::make_unique<detail::Component<T>>(this, std::move(name));
        auto index = comp->type();
        m_components.emplace(index, std::move(comp));
    }

    template <typename Interface, std::derived_from<Interface> T>
    void declareInterface () {
        // Only empty archetype should exist for interface declaration to be correct
        PHENYL_ASSERT_MSG(m_archetypes.size() == 1, "Must declare interfaces before any entities are created!");

        auto interfaceType = meta::TypeIndex::Get<std::remove_cvref_t<Interface>>();
        auto compType = meta::TypeIndex::Get<std::remove_cvref_t<T>>();

        auto it = m_components.find(compType);
        PHENYL_ASSERT_MSG(it != m_components.end(), "Cannot declare interface of non-existent component");

        it->second->declareInterface(interfaceType);
    }

    Entity create (EntityId parent = EntityId{});
    void remove (EntityId id);
    void reparent (EntityId id, EntityId parent);

    void clear ();

    [[nodiscard]] bool exists (EntityId id) const noexcept {
        return m_idList.check(id);
    }

    Entity entity (EntityId id) noexcept;
    ChildrenView root () noexcept;

    [[nodiscard]] Entity parent (EntityId id) noexcept;

    template <typename... Args>
    Query<Args...> query () {
        return Query<Args...>{makeQueryArchetypes(detail::ArchetypeKey::Make<Args...>()), this};
    }

    template <typename T>
    void addHandler (std::function<void(const OnInsert<T>&, Entity)> handler) {
        auto it = m_components.find(meta::TypeIndex::Get<T>());
        PHENYL_ASSERT_MSG(it != m_components.end(), "Failed to find component in addHandler()");

        auto& component = static_cast<detail::Component<T>&>(*it->second);
        component.addHandler(std::move(handler));
    }

    template <typename T>
    void addHandler (std::function<void(const OnRemove<T>&, Entity)> handler) {
        auto it = m_components.find(meta::TypeIndex::Get<T>());
        PHENYL_ASSERT_MSG(it != m_components.end(), "Failed to find component in addHandler()");

        auto& component = static_cast<detail::Component<T>&>(*it->second);
        component.addHandler(std::move(handler));
    }

    template <typename Signal, typename... Args>
    void addHandler (std::function<void(const Signal&, const Bundle<Args...>& bundle)> handler) {
        detail::SignalHandlerVector<Signal>* handlerVec;
        auto vecIt = m_signalHandlerVectors.find(meta::TypeIndex::Get<Signal>());
        if (vecIt != m_signalHandlerVectors.end()) {
            handlerVec = static_cast<detail::SignalHandlerVector<Signal>*>(vecIt->second.get());
        } else {
            auto newVec = std::make_unique<detail::SignalHandlerVector<Signal>>(*this);
            handlerVec = newVec.get();
            m_signalHandlerVectors.emplace(meta::TypeIndex::Get<Signal>(), std::move(newVec));
        }

        handlerVec->addHandler(
            std::make_unique<detail::SignalHandler2<Signal, Args...>>(query<Args...>(), std::move(handler)));
    }

    template <typename Signal, typename... Args>
    void addHandler (std::invocable<const Signal&, const Bundle<Args...>&> auto&& fn) {
        addHandler<Signal, Args...>(
            std::function<void(const Signal&, const Bundle<Args...>&)>{std::forward<decltype(fn)>(fn)});
    }

    template <typename Signal, typename... Args>
    void addHandler (std::function<void(const Signal&, std::remove_reference_t<Args>&... args)> handler) {
        addHandler(std::function<void(const Signal&, const Bundle<Args...>&)>{
          [handler = std::move(handler)] (const Signal& signal, const Bundle<Args...>& bundle) {
              handler(signal, bundle.template get<Args>()...);
          }});
    }

    template <typename Signal, typename... Args>
    void addHandler (std::invocable<const Signal&, std::remove_reference_t<Args>&...> auto&& fn) {
        addHandler<Signal, Args...>(
            std::function<void(const Signal&, std::remove_reference_t<Args>&...)>{std::forward<decltype(fn)>(fn)});
    }

    void defer ();
    void deferEnd ();
    void deferSignals ();
    void deferSignalsEnd ();

    PrefabBuilder buildPrefab ();

    iterator begin ();
    iterator end ();

private:
    std::unordered_map<meta::TypeIndex, std::unique_ptr<detail::UntypedComponent>> m_components;

    detail::EntityIdList m_idList;
    detail::RelationshipManager m_relationships;

    std::vector<std::unique_ptr<Archetype>> m_archetypes;
    EmptyArchetype* m_emptyArchetype;
    std::vector<detail::EntityEntry> m_entityEntries;

    std::vector<std::weak_ptr<QueryArchetypes>> m_queryArchetypes;

    std::unordered_map<meta::TypeIndex, std::unique_ptr<detail::IHandlerVector>> m_signalHandlerVectors;

    std::shared_ptr<PrefabManager> m_prefabManager;

    std::vector<std::pair<EntityId, EntityId>> m_deferredCreations;
    std::vector<std::pair<EntityId, std::function<void(Entity)>>> m_deferredApplys;
    std::vector<EntityId> m_deferredRemovals;

    std::uint32_t m_deferCount = 0;
    std::uint32_t m_removeDeferCount = 0;
    std::uint32_t m_signalDeferCount = 0;

    void completeCreation (EntityId id, EntityId parent);
    void removeInt (EntityId id, bool updateParent);

    std::shared_ptr<QueryArchetypes> makeQueryArchetypes (detail::ArchetypeKey key);
    void cleanupQueryArchetypes ();

    detail::UntypedComponent* findComponent (meta::TypeIndex compType) override;
    Archetype* findArchetype (const detail::ArchetypeKey& key) override;
    void updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) override;

    void onComponentInsert (EntityId id, meta::TypeIndex compType, std::byte* ptr) override;
    void onComponentRemove (EntityId id, meta::TypeIndex compType, std::byte* ptr) override;

    void deferInsert (EntityId id, meta::TypeIndex compType, std::byte* ptr);
    void deferErase (EntityId id, meta::TypeIndex compType);
    void deferApply (EntityId id, std::function<void(Entity)> applyFunc);

    void instantiatePrefab (EntityId id, const detail::PrefabFactories& factories);

    void raiseSignal (EntityId id, meta::TypeIndex signalType, std::byte* ptr);

    void deferRemove ();
    void deferRemoveEnd ();

    friend Entity;
    friend ChildrenView;
    friend PrefabManager;
};
} // namespace phenyl::core
