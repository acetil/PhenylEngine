#pragma once

#include "archetype.h"
#include "archetype_view.h"
#include "children_view.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace phenyl::core {
namespace detail {
    class QueryKey {
    public:
        QueryKey (ArchetypeKey archKey, std::vector<meta::TypeIndex> interfaces);

        bool isSatisfied (const Archetype* archetype);

        bool operator== (const QueryKey& other) const;

    private:
        ArchetypeKey m_archKey;
        std::vector<meta::TypeIndex> m_interfaces;
    };
} // namespace detail

class QueryArchetypes {
public:
    explicit QueryArchetypes (World& world, detail::QueryKey key);

    class Iterator {
    public:
        using value_type = Archetype;
        using reference = Archetype&;
        using difference_type = std::ptrdiff_t;

        Iterator ();

        reference operator* () const;

        Iterator& operator++ ();
        Iterator operator++ (int);

        // Iterator& operator-- ();
        // Iterator operator-- (int);

        bool operator== (const Iterator&) const noexcept;

    private:
        std::unordered_set<Archetype*>::const_iterator m_it;

        Iterator (std::unordered_set<Archetype*>::const_iterator it);
        friend QueryArchetypes;
    };

    using const_iterator = Iterator;
    using iterator = Iterator;

    const detail::QueryKey& getKey () const noexcept {
        return m_key;
    }

    void onNewArchetype (Archetype* archetype);

    iterator begin () {
        return iterator{m_archetypes.begin()};
    }

    iterator end () {
        return iterator{m_archetypes.end()};
    }

    const_iterator begin () const {
        return cbegin();
    }

    const_iterator cbegin () const {
        return const_iterator{m_archetypes.begin()};
    }

    const_iterator end () const {
        return cend();
    }

    const_iterator cend () const {
        return const_iterator{m_archetypes.end()};
    }

    bool contains (Archetype* archetype) const noexcept {
        return m_archetypes.contains(archetype);
    }

    void lock ();
    void unlock ();

private:
    World& m_world;
    detail::QueryKey m_key;
    std::unordered_set<Archetype*> m_archetypes;
};

template <typename F, typename... Args> concept Query2Callback = std::invocable<F, std::remove_reference_t<Args>&...>;

template <typename F, typename... Args> concept Query2PairCallback =
    std::invocable<F, const Bundle<Args...>&, const Bundle<Args...>&>;

template <typename F, typename... Args> concept QueryHierachicalCallback =
    std::invocable<F, const Bundle<Args...>*, const Bundle<Args...>&>;

template <typename F, typename... Args> concept Query2BundleCallback = std::invocable<F, const Bundle<Args...>&>;

template <typename... Args>
class Query {
public:
    Query () : m_archetypes{nullptr} {}

    explicit operator bool () const noexcept {
        return static_cast<bool>(m_archetypes);
    }

    void each (const Query2Callback<Args...> auto& fn) const {
        PHENYL_DASSERT(*this);
        m_archetypes->lock();
        for (auto& archetype : *m_archetypes) {
            for (auto comps : ArchetypeView<Args...>{archetype, m_world}) {
                fn(std::get<std::remove_reference_t<Args>&>(comps)...);
            }
        }
        m_archetypes->unlock();
    }

    void each (const Query2BundleCallback<Args...> auto& fn) const {
        PHENYL_DASSERT(*this);
        m_archetypes->lock();
        for (auto& archetype : *m_archetypes) {
            ArchetypeView<Args...> view{archetype, m_world};
            for (const auto& bundle : view.bundles()) {
                fn(bundle);
            }
        }
        m_archetypes->unlock();
    }

    void entity (Entity entity, const Query2BundleCallback<Args...> auto& fn) const {
        PHENYL_DASSERT(*this);
        // const auto& entry = entity.entry();
        //
        // if (!m_archetypes->contains(entry.archetype)) {
        //     return;
        // }
        //
        // ArchetypeView<Args...> view{*entry.archetype, m_world};
        // fn(view.bundle(entry.pos));
        auto bundle = entityBundle(entity);
        if (bundle) {
            fn(*bundle);
        }
    }

    void pairs (const Query2PairCallback<Args...> auto& fn) const {
        PHENYL_DASSERT(*this);

        m_archetypes->lock();
        for (auto a1It = m_archetypes->begin(); a1It != m_archetypes->end(); ++a1It) {
            ArchetypeView<Args...> view1{*a1It, m_world};
            pairsIter(fn, view1);

            for (auto a2It = std::next(a1It); a2It != m_archetypes->end(); ++a2It) {
                ArchetypeView<Args...> view2{*a2It, m_world};
                pairsIter2(fn, view1, view2);
            }
        }
        m_archetypes->unlock();
    }

    void hierarchical (const QueryHierachicalCallback<Args...> auto& fn) const {
        PHENYL_DASSERT(*this);
        m_archetypes->lock();
        hierarchicalIter(fn, Entity{EntityId{}, m_world}, nullptr);
        m_archetypes->unlock();
    }

private:
    std::shared_ptr<QueryArchetypes> m_archetypes;
    World* m_world;

    explicit Query (std::shared_ptr<QueryArchetypes> archetypes, World* world) :
        m_archetypes{std::move(archetypes)},
        m_world{world} {}
    friend class World;

    void pairsIter (const Query2PairCallback<Args...> auto& fn, ArchetypeView<Args...>& view) const {
        // Iterate though pairs within archetype
        auto bundles = view.bundles();
        for (auto b1It = bundles.begin(); b1It != bundles.end(); ++b1It) {
            const auto& b1 = *b1It;
            for (auto b2It = std::next(b1It); b2It != bundles.end(); ++b2It) {
                fn(b1, *b2It);
            }
        }
    }

    void pairsIter2 (const Query2PairCallback<Args...> auto& fn, ArchetypeView<Args...>& view1,
        ArchetypeView<Args...>& view2) const {
        // Iterate through pairs in different archetypes
        auto bundles1 = view1.bundles();
        auto bundles2 = view2.bundles();

        for (const auto& b1 : bundles1) {
            for (const auto& b2 : bundles2) {
                fn(b1, b2);
            }
        }
    }

    void hierarchicalIter (const QueryHierachicalCallback<Args...> auto& fn, Entity parent,
        const Bundle<Args...>* parentBundle) const {
        for (auto child : parent.children()) {
            auto childBundle = entityBundle(child);
            if (!childBundle) {
                continue;
            }

            fn(parentBundle, *childBundle);
            hierarchicalIter(fn, child, &*childBundle);
        }
    }

    std::optional<Bundle<Args...>> entityBundle (Entity entity) const {
        const auto& entry = entity.entry();
        auto* archetype = entry.archetype;
        if (!m_archetypes->contains(archetype)) {
            return std::nullopt;
        }

        return ArchetypeView<Args...>{*archetype, m_world}.bundle(entry.pos);
    }
};
} // namespace phenyl::core
