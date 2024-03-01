#pragma once

#include "component/forward.h"
#include "component/detail/managers/basic_manager.h"
#include "component/entity.h"
#include "component/query.h"

namespace phenyl::component {
    class ChildrenView {
    private:
        class Iterator {
        private:
            detail::RelationshipManager::ChildIterator it;
            detail::BasicManager* manager;

            Iterator (detail::BasicManager* manager, detail::RelationshipManager::ChildIterator it) : manager{manager}, it{it} {}
            friend ChildrenView;
        public:
            using value_type = Entity;
            using difference_type = std::ptrdiff_t;
            Iterator () : it{}, manager{nullptr} {}

            value_type operator* () const {
                return manager->_entity(*it);
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
            const detail::BasicManager* manager;

            ConstIterator (const detail::BasicManager* manager, detail::RelationshipManager::ChildIterator it) : manager{manager}, it{it} {}

            friend ChildrenView;
        public:
            using value_type = ConstEntity;
            using difference_type = std::ptrdiff_t;
            ConstIterator () : it{}, manager{nullptr} {}

            value_type operator* () const {
                return manager->_entity(*it);
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
        detail::BasicManager* manager;


        template <typename ...Args, QueryCallback<Args...> F, std::size_t ...Indexes>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) {
            for (auto i : manager->getRelationshipManager().children(parentId)) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(i)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    fn(manager->_entity(i), *std::get<Indexes>(tup)...);
                }
            }
        }

        template <typename ...Args, ConstQueryCallback<Args...> F, std::size_t ...Indexes>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) const {
            for (auto i : manager->getRelationshipManager().children(parentId)) {
                auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(i)...);
                if (detail::tupleAllNonNull<Args...>(tup)) {
                    fn(manager->_entity(i), *std::get<Indexes>(tup)...);
                }
            }
        }

        ChildrenView (detail::BasicManager* manager, EntityId parent) : manager{manager}, parentId{parent} {}
        friend ComponentManager;
        friend detail::BasicManager;
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

        template <typename ...Args, QueryCallback<Args...> F>
        void each (F fn) {
            auto compSets = std::array{manager->getComponent<Args>()...};
            for (auto i = 0; i < sizeof...(Args); i++) {
                if (!compSets[i]) {
                    PHENYL_LOGE(detail::MANAGER_LOGGER, "Attempted to call each() with component that hasn't been added yet!");
                    return;
                }
            }

            eachInt<Args...>(compSets, fn, std::make_index_sequence<sizeof...(Args)>{});
        }

        template <typename ...Args, ConstQueryCallback<Args...> F>
        void each (F fn) const {
            auto compSets = std::array{manager->getComponent<Args>()...};
            for (auto i = 0; i < sizeof...(Args); i++) {
                if (!compSets[i]) {
                    PHENYL_LOGE(detail::MANAGER_LOGGER, "Attempted to call each() with component that hasn't been added yet!");
                    return;
                }
            }

            eachInt<Args...>(compSets, fn, std::make_index_sequence<sizeof...(Args)>{});
        }

        Entity parent () {
            return manager->_entity(parentId);
        }

        [[nodiscard]] ConstEntity parent () const {
            return manager->_entity(parentId);
        }
    };
}