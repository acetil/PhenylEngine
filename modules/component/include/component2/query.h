#pragma once

#include <memory>
#include <vector>

#include "archetype.h"
#include "archetype_view.h"

namespace phenyl::component {
    class QueryArchetypes {
    private:
        std::vector<std::size_t> componentIds;
        std::vector<Archetype*> archetypes;

    public:
        explicit QueryArchetypes (std::vector<std::size_t> componentIds);
        class Iterator {
        private:
            std::vector<Archetype*>::const_iterator it;

            Iterator (std::vector<Archetype*>::const_iterator it);
            friend QueryArchetypes;
        public:
            using value_type = Archetype;
            using reference = Archetype&;
            using difference_type = std::ptrdiff_t;

            Iterator ();

            reference operator* () const;

            Iterator& operator++ ();
            Iterator operator++ (int);

            Iterator& operator-- ();
            Iterator operator-- (int);

            bool operator== (const Iterator&) const noexcept;
        };

        using const_iterator = Iterator;
        using iterator = Iterator;

        const std::vector<std::size_t>& components () const noexcept {
            return componentIds;
        }

        void onNewArchetype (Archetype* archetype);

        iterator begin () {
            return iterator{archetypes.begin()};
        }

        iterator end () {
            return iterator{archetypes.end()};
        }

        const_iterator begin () const {
            return cbegin();
        }
        const_iterator cbegin () const {
            return const_iterator{archetypes.begin()};
        }

        const_iterator end () const {
            return cend();
        }
        const_iterator cend () const {
            return const_iterator{archetypes.end()};
        }
    };

    template <typename F, typename ...Args>
    concept Query2Callback = meta::callable<F, void, std::remove_reference_t<Args>&...>;

    template <typename F, typename ...Args>
    concept Query2PairCallback = meta::callable<F, void, const Bundle<Args...>&, const Bundle<Args...>&>;

    template <typename ...Args>
    class Query2 {
    private:
        std::shared_ptr<QueryArchetypes> archetypes;
        ComponentManager2* manager;

        explicit Query2 (std::shared_ptr<QueryArchetypes> archetypes, ComponentManager2* manager) : archetypes{std::move(archetypes)}, manager{manager} {}
        friend class ComponentManager2;

        void pairsIter (const Query2PairCallback<Args...> auto& fn, ArchetypeView<Args...>& view) {
            util::Iterable<typename ArchetypeView<Args...>::BundleIterator> bundles = view.bundles();
            for (auto b1It = bundles.begin(); b1It != bundles.end(); ++b1It) {
                const auto& b1 = *b1It;
                for (const auto& b2It = std::next(b1It); b2It != bundles.end(); ++b2It) {
                    fn(b1, *b2It);
                }
            }
        }

        void pairsIter2 (const Query2PairCallback<Args...> auto& fn, ArchetypeView<Args...>& view1, ArchetypeView<Args...>& view2) {
            util::Iterable<typename ArchetypeView<Args...>::BundleIterator> bundles1 = view1.bundles();
            util::Iterable<typename ArchetypeView<Args...>::BundleIterator> bundles2 = view2.bundles();

            for (const auto& b1 : bundles1) {
                for (const auto& b2 : bundles2) {
                    fn(b1, b2);
                }
            }
        }
    public:
        Query2 () : archetypes{nullptr} {}

        explicit operator bool () const noexcept {
            return static_cast<bool>(archetypes);
        }

        void each (const Query2Callback<Args...> auto& fn) const {
            for (auto& archetype : *archetypes) {
                for (auto comps : ArchetypeView<Args...>{archetype, manager}) {
                    fn(std::get<std::remove_cvref_t<Args>&>(comps)...);
                }
            }
        }

        void pairs (const Query2PairCallback<Args...> auto& fn) const {
            for (auto a1It = archetypes->begin(); a1It != archetypes->end(); ++a1It) {
                pairsIter(fn, *a1It);

                ArchetypeView<Args...> view1{*a1It, manager};
                for (auto a2It = std::next(a1It); a2It != archetypes->end(); ++a2It) {
                    pairsIter2(fn, view1, ArchetypeView<Args...>{*a2It, manager});
                }
            }
        }
    };
}
