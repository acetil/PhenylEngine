#pragma once

#include "component/forward.h"
#include "basic_manager.h"
#include "component/entity.h"

namespace component {
    class ChildrenView {
    private:
        class Iterator {
        private:
            detail::RelationshipManager::ChildIterator it;
            detail::BasicComponentManager* manager;

            Iterator (detail::BasicComponentManager* manager, detail::RelationshipManager::ChildIterator it) : manager{manager}, it{it} {}
            friend ChildrenView;
        public:
            using value_type = Entity;
            using difference_type = std::ptrdiff_t;
            Iterator () : it{}, manager{nullptr} {}

            value_type operator* () const {
                return manager->_view(*it);
            }

            Iterator& operator++ () {
                ++it;
                return *this;
            }

            Iterator operator++ (int) {
                Iterator newIt = *this;
                ++*this;
                return newIt;
            }

            bool operator== (const Iterator& other) const {
                return manager == other.manager && it == other.it;
            }
        };

        class ConstIterator {
        private:
            detail::RelationshipManager::ChildIterator it;
            const detail::BasicComponentManager* manager;

            ConstIterator (const detail::BasicComponentManager* manager, detail::RelationshipManager::ChildIterator it) : manager{manager}, it{it} {}

            friend ChildrenView;
        public:
            using value_type = ConstEntity;
            using difference_type = std::ptrdiff_t;
            ConstIterator () : it{}, manager{nullptr} {}

            value_type operator* () const {
                return manager->_view(*it);
            }

            ConstIterator& operator++ () {
                ++it;
                return *this;
            }

            ConstIterator operator++ (int) {
                ConstIterator newIt = *this;
                ++*this;
                return newIt;
            }

            bool operator== (const ConstIterator& other) const {
                return manager == other.manager && it == other.it;
            }
        };

        EntityId parentId;
        detail::BasicComponentManager* manager;


        template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F, std::size_t ...Indexes>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) {
            /*auto curr = manager->getRelationship(parentId).children;

            while (curr) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(curr)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    IterInfo info{manager, curr};
                    fn(info, *std::get<Indexes>(tup)...);
                }

                curr = manager->getRelationship(curr).next;
            }*/
            for (auto i : manager->getRelationshipManager().children(parentId)) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(i)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    IterInfo info{manager->asManager(), i};
                    fn(info, *std::get<Indexes>(tup)...);
                }
            }
        }

        template <typename ...Args, meta::callable<void, const IterInfo&, const Args&...> F, std::size_t ...Indexes>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) const {
            /*auto curr = manager->getRelationship(parentId).children;

            while (curr) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(curr)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    IterInfo info{manager, curr};
                    fn(info, *std::get<Indexes>(tup)...);
                }

                curr = manager->getRelationship(curr).next;
            }*/
            for (auto i : manager->getRelationshipManager().children(parentId)) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(i)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    IterInfo info{manager->asManager(), i};
                    fn(info, *std::get<Indexes>(tup)...);
                }
            }
        }

        ChildrenView (detail::BasicComponentManager* manager, EntityId parent) : manager{manager}, parentId{parent} {}
        friend ComponentManager;
        friend detail::BasicComponentManager;
        friend Entity;
    public:
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        static_assert(std::forward_iterator<iterator>);
        static_assert(std::forward_iterator<const_iterator>);

        iterator begin () {
            return iterator{manager, manager->getRelationshipManager().childrenBegin(parentId)};
        }
        iterator end () {
            return iterator{manager, manager->getRelationshipManager().childrenEnd(parentId)};
        }

        [[nodiscard]] const_iterator begin () const {
            return cbegin();
        }
        [[nodiscard]] const_iterator cbegin () const {
            return const_iterator{manager, manager->getRelationshipManager().childrenBegin(parentId)};
        }

        [[nodiscard]] const_iterator end () const {
            return cend();
        }
        [[nodiscard]] const_iterator cend () const {
            return const_iterator{manager, manager->getRelationshipManager().childrenEnd(parentId)};
        }

        template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F>
        void each (F fn) {
            auto compSets = std::array{manager->getComponent<Args>()...};
            for (auto i = 0; i < sizeof...(Args); i++) {
                if (!compSets[i]) {
                    logging::log(LEVEL_ERROR, "Attempted to call each() with component that hasn't been added yet!");
                    return;
                }
            }

            eachInt<Args...>(compSets, fn, std::make_index_sequence<sizeof...(Args)>{});
        }

        template <typename ...Args, meta::callable<void, const IterInfo&, const Args&...> F>
        void each (F fn) const {
            auto compSets = std::array{manager->getComponent<Args>()...};
            for (auto i = 0; i < sizeof...(Args); i++) {
                if (!compSets[i]) {
                    logging::log(LEVEL_ERROR, "Attempted to call each() with component that hasn't been added yet!");
                    return;
                }
            }

            eachInt<Args...>(compSets, fn, std::make_index_sequence<sizeof...(Args)>{});
        }

        Entity parent () {
            return manager->_view(parentId);
        }

        [[nodiscard]] ConstEntity parent () const {
            return manager->_view(parentId);
        }
    };
}