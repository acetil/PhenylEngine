#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "archetype.h"
#include "archetype_view.h"

namespace phenyl::core {
    class QueryArchetypes {
    private:
        World& m_world;
        detail::ArchetypeKey m_key;
        std::unordered_set<Archetype*> m_archetypes;

    public:
        explicit QueryArchetypes (World& world, detail::ArchetypeKey key);
        class Iterator {
        private:
            std::unordered_set<Archetype*>::const_iterator m_it;

            Iterator (std::unordered_set<Archetype*>::const_iterator it);
            friend QueryArchetypes;
        public:
            using value_type = Archetype;
            using reference = Archetype&;
            using difference_type = std::ptrdiff_t;

            Iterator ();

            reference operator* () const;

            Iterator& operator++ ();
            Iterator operator++ (int);

            //Iterator& operator-- ();
            //Iterator operator-- (int);

            bool operator== (const Iterator&) const noexcept;
        };

        using const_iterator = Iterator;
        using iterator = Iterator;

        const detail::ArchetypeKey& getKey () const noexcept {
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
    };

    template <typename F, typename ...Args>
    concept Query2Callback = meta::callable<F, void, std::remove_reference_t<Args>&...>;

    template <typename F, typename ...Args>
    concept Query2PairCallback = meta::callable<F, void, const Bundle<Args...>&, const Bundle<Args...>&>;

    template <typename F, typename ...Args>
    concept Query2BundleCallback = meta::callable<F, void, const Bundle<Args...>&>;

    template <typename ...Args>
    class Query {
    private:
        std::shared_ptr<QueryArchetypes> m_archetypes;
        World* m_world;

        explicit Query (std::shared_ptr<QueryArchetypes> archetypes, World* world) : m_archetypes{std::move(archetypes)}, m_world{world} {}
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

        void pairsIter2 (const Query2PairCallback<Args...> auto& fn, ArchetypeView<Args...>& view1, ArchetypeView<Args...>& view2) const {
            // Iterate through pairs in different archetypes
            auto bundles1 = view1.bundles();
            auto bundles2 = view2.bundles();

            for (const auto& b1 : bundles1) {
                for (const auto& b2 : bundles2) {
                    fn(b1, b2);
                }
            }
        }
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
            const auto& entry = entity.entry();

            if (!m_archetypes->contains(entry.archetype)) {
                return;
            }

            ArchetypeView<Args...> view{*entry.archetype, m_world};
            fn(view.bundle(entry.pos));
        }

        void pairs (const Query2PairCallback<Args...> auto& fn) const {
            PHENYL_DASSERT(*this);

            m_archetypes->lock();
            for (auto a1It = m_archetypes->begin(); a1It != m_archetypes->end(); ++a1It) {
                ArchetypeView<Args...> view1{*a1It, m_world};
                pairsIter(fn, view1);

                for (auto a2It = std::next(a1It); a2It != m_archetypes->end(); ++a2It) {
                    ArchetypeView<Args...> view2{*a2It, m_world} ;
                    pairsIter2(fn, view1, view2);
                }
            }
            m_archetypes->unlock();
        }
    };
}
