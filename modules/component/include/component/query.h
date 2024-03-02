#pragma once

#include <array>
#include <iterator>
#include <numeric>

#include "detail/component_set.h"
#include "detail/managers/basic_manager.h"
#include "entity.h"

#include "util/iterable.h"
#include "util/meta.h"

namespace phenyl::component {
    namespace detail {
        template <typename ...Args>
        class QueryCursor;
    }

    template <typename ...Args>
    class Query;
    template <typename ...Args>
    class ConstQuery;

    template <typename ...Args>
    class QueryBundle {
    private:
        Entity bundleEntity;
        std::tuple<Args&...> comps;

        QueryBundle (Entity entity, Args&... args) : bundleEntity{entity}, comps{args...} {}

        friend detail::QueryCursor<Args...>;
    public:
        [[nodiscard]] Entity entity () const {
            return bundleEntity;
        }

        template <typename T>
        T& get () const {
            return std::get<T&>(comps);
        }
    };

    template <typename ...Args>
    class ConstQueryBundle {
    private:
        ConstEntity bundleEntity;
        std::tuple<const Args&...> comps;

        ConstQueryBundle (ConstEntity entity, const Args&... args) : bundleEntity{entity}, comps{args...} {}

        friend detail::QueryCursor<const Args...>;
    public:
        [[nodiscard]] ConstEntity entity () const {
            return bundleEntity;
        }

        template <typename T>
        T& get () const {
            return std::get<T&>(comps);
        }
    };

    template <typename F, typename ...Args>
    concept QueryCallback = meta::callable<F, void, Entity, std::remove_reference_t<Args>&...>;
    template <typename F, typename ...Args>
    concept ConstQueryCallback = meta::callable<F, void, ConstEntity, const std::remove_cvref_t<Args>&...>;

    template <typename F, typename ...Args>
    concept QueryPairCallback = meta::callable<F, void, const QueryBundle<Args...>&, const QueryBundle<Args...>&>;
    template <typename F, typename ...Args>
    concept ConstQueryPairCallback = meta::callable<F, void, const ConstQueryBundle<Args...>&, const ConstQueryBundle<Args...>&>;


    namespace detail {
        class QueryableManager;
        template <typename ...Args>
        class QueryCursorIterator;

        template <typename ...Args>
        inline util::Iterable<QueryCursorIterator<Args...>> CursorChildren (const QueryCursor<Args...>& cursor);

        template <typename ...Args>
        class QueryCursor {
        private:
            std::size_t pos{0};

            QueryCursor (const std::array<detail::ComponentSet*, sizeof...(Args)>* compSets, detail::ComponentSet* primarySet) : compSets{compSets}, primarySet{primarySet} {
                PHENYL_DASSERT(compSets);
                advanceToFirst();
            }

            explicit QueryCursor (const std::array<detail::ComponentSet*, sizeof...(Args)>* compSets) : compSets{compSets}, primarySet{nullptr} {
                PHENYL_DASSERT(compSets);
                auto minSize = std::numeric_limits<std::size_t>::max();

                for (detail::ComponentSet* i : *compSets) {
                    if (i->totalSize() < minSize) {
                        minSize = i->totalSize();
                        primarySet = i;
                    }
                }

                advanceToFirst();
            }

            [[nodiscard]] bool validPos () const {
                auto id = primarySet->idAtIndex(pos);

                for (detail::ComponentSet* i : *compSets) {
                    if (!i->getComponentUntyped(id)) {
                        return false;
                    }
                }

                return true;
            }

            void advanceToFirst () {
                while (pos < primarySet->size() && !validPos()) {
                    pos++;
                }
            }

            [[nodiscard]] EntityId currId () const {
                return primarySet->idAtIndex(pos);
            }

            template <std::size_t ...Indexes>
            void applyInt (QueryCallback<Args...> auto& fn, Entity entity, std::index_sequence<Indexes...>) const {
                fn(entity, (*(*compSets)[Indexes]->template getComponent<Args>(currId()))...);
            }

            template <std::size_t ...Indexes>
            QueryBundle<Args...> bundleInt (detail::BasicManager* manager, std::index_sequence<Indexes...>) const {
                return QueryBundle<Args...>{manager->_entity(currId(), (*(*compSets)[Indexes]->template getComponent<Args>(currId()))...)};
            }

            friend class Query<Args...>;
            friend class ConstQuery<Args...>;
            friend class QueryCursorIterator<Args...>;
            friend util::Iterable<QueryCursorIterator<Args...>> CursorChildren<> (const QueryCursor<Args...>& cursor);
        public:
            const std::array<detail::ComponentSet*, sizeof...(Args)>* compSets;
            detail::ComponentSet* primarySet;
            explicit operator bool () const {
                return primarySet;
            }

            [[nodiscard]] bool done () const {
                return pos >= primarySet->size();
            }

            void apply (QueryCallback<Args...> auto& fn, detail::BasicManager* manager) const {
                applyInt(fn, manager->_entity(currId()), std::index_sequence_for<Args...>{});
            }

            void next () {
                pos++;
                advanceToFirst();
            }

            QueryBundle<Args...> bundle (detail::BasicManager* manager) const {
                return bundleInt(manager, std::index_sequence_for<Args...>{});
            }
        };

        template <typename T>
        class QueryCursor<T> {
        private:
            std::size_t pos{0};

            QueryCursor (detail::ComponentSet* compSet) : compSet{compSet} {}

            [[nodiscard]] EntityId currId () const {
                return compSet->idAtIndex(pos);
            }

            friend class Query<T>;
            friend class ConstQuery<T>;
            friend class QueryCursorIterator<T>;
            friend util::Iterable<QueryCursorIterator<T>> CursorChildren<> (const QueryCursor<T>& cursor);
        public:
            detail::ComponentSet* compSet;
            explicit operator bool () const {
                return compSet;
            }

            [[nodiscard]] bool done () const {
                return pos >= compSet->size();
            }

            void apply (QueryCallback<T> auto& fn, detail::BasicManager* manager) const {
                fn(manager->_entity(currId()), *((T*)compSet->getAtIndex(pos)));
            }

            void next () {
                pos++;
            }

            QueryBundle<T> bundle (detail::BasicManager* manager) const {
                return QueryBundle<T>{manager->_entity(currId()), *((T*)compSet->getAtIndex(pos))};
            }
        };

        template <typename ...Args>
        class QueryCursorIterator {
        private:
            const std::array<detail::ComponentSet*, sizeof...(Args)>* compSets;

            friend QueryCursor<Args...>;

            friend util::Iterable<QueryCursorIterator<Args...>> CursorChildren<> (const QueryCursor<Args...>& cursor);
        public:
            QueryCursorIterator (const std::array<detail::ComponentSet*, sizeof...(Args)>* compSets, detail::ComponentSet* curr) : compSets{compSets}, curr{curr} {}
            detail::ComponentSet* curr;
            using value_type = QueryCursor<Args...>;
            using difference_type = std::ptrdiff_t;

            QueryCursorIterator () : compSets{nullptr}, curr{nullptr} {}

            value_type operator* () const {
                return QueryCursor<Args...>{compSets, curr};
            }

            QueryCursorIterator<Args...>& operator++ () {
                curr = curr->getNextChild();
                return *this;
            }

            QueryCursorIterator<Args...> operator++ (int) {
                auto it = *this;
                ++*this;
                return it;
            }

            bool operator== (const QueryCursorIterator<Args...>& other) const {
                return compSets == other.compSets && curr == other.curr;
            }
        };

        template <typename T>
        class QueryCursorIterator<T> {
        private:
            detail::ComponentSet* curr;
            friend QueryCursor<T>;
            friend util::Iterable<QueryCursorIterator<T>> CursorChildren<> (const QueryCursor<T>& cursor);
        public:
            explicit QueryCursorIterator (detail::ComponentSet* curr) : curr{curr} {}
            using value_type = QueryCursor<T>;
            using difference_type = std::ptrdiff_t;

            QueryCursorIterator () : curr{nullptr} {}
            value_type operator* () const {
                return QueryCursor<T>{curr};
            }

            QueryCursorIterator<T>& operator++ () {
                curr = curr->getNextChild();
                return *this;
            }

            QueryCursorIterator<T> operator++ (int) {
                auto it = *this;
                ++*this;
                return it;
            }

            bool operator== (const QueryCursorIterator<T>& other) const {
                return curr == other.curr;
            }
        };

        static_assert(std::forward_iterator<QueryCursorIterator<int, float>>);
        static_assert(std::forward_iterator<QueryCursorIterator<int>>);

        template <typename ...Args>
        util::Iterable<QueryCursorIterator<Args...>> CursorChildren (const QueryCursor<Args...>& cursor) requires (sizeof...(Args) > 1){
            auto begin = QueryCursorIterator<Args...>(cursor.compSets, cursor.primarySet->getChildren());
            auto end = QueryCursorIterator<Args...>(cursor.compSets, nullptr);

            return {begin, end};
        }

        template <typename T>
        util::Iterable<QueryCursorIterator<T>> CursorChildren (const QueryCursor<T>& cursor) {
            auto begin = QueryCursorIterator<T>(cursor.compSet->getChildren());
            auto end = QueryCursorIterator<T>(nullptr);

            return {begin, end};
        }

        template <typename ...Args>
        class QuerySets {
        private:
            std::array<detail::ComponentSet*, sizeof...(Args)> sets;

            template <std::size_t ...Indexes>
            void singleInt (QueryCallback<Args...> auto& fn, Entity entity, std::index_sequence<Indexes...>) {
                std::tuple<Args*...> tup = std::make_tuple(sets[Indexes]->template getComponent<Args>(entity.id())...);
                if (tupleAllNonNull<Args...>(tup)) {
                    fn(entity, (*std::get<Args*>(tup))...);
                }
            }
        public:
            using CursorRef = const std::array<detail::ComponentSet*, sizeof...(Args)>*;

            QuerySets () : sets{} {}

            template <std::same_as<detail::ComponentSet> ...Args2>
            explicit QuerySets (Args2*... args) requires (sizeof...(Args2) == sizeof...(Args)) : sets{args...} {}

            [[nodiscard]] CursorRef getCursorRef () const {
                return &sets;
            }

            void single (QueryCallback<Args...> auto& fn, Entity entity) {
                singleInt(fn, entity, std::make_index_sequence<sizeof...(Args)>{});
            }
        };

        template <typename T>
        class QuerySets<T> {
        private:
            detail::ComponentSet* set;
        public:
            using CursorRef = detail::ComponentSet*;
            QuerySets () : set{nullptr} {}
            explicit QuerySets (detail::ComponentSet* set) : set{set} {}

            [[nodiscard]] CursorRef getCursorRef () const {
                return set;
            }

            void single (QueryCallback<T> auto& fn, Entity entity) {
                T* comp = set->getComponent<T>();

                if (comp) {
                    fn(entity, *comp);
                }
            }
        };
    }

    template <typename ...Args>
    class Query {
    private:
        detail::BasicManager* manager;
        detail::QuerySets<Args...> querySets;

        template <std::same_as<detail::ComponentSet> ...Args2>
        Query (detail::BasicManager* manager, Args2*... args) requires (sizeof...(Args) == sizeof...(Args2)) : manager{manager}, querySets{args...} {
            PHENYL_DASSERT(manager);
        }

        void eachInt (QueryCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                cursor.apply(fn, manager);
                cursor.next();
            }

            for (auto i : detail::CursorChildren(cursor)) {
                eachInt(fn, i);
            }
        }

        void pairsIntInner (QueryPairCallback<Args...> auto& fn, const QueryBundle<Args...>& bundle1, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                auto bundle2 = cursor.bundle(manager);
                fn(bundle1, bundle2);
                cursor.next();
            }

            for (auto i : detail::CursorChildren(cursor)) {
                pairsIntInner(fn, bundle1, i);
            }
        }

        void pairsIntDouble (QueryPairCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor1, detail::QueryCursor<Args...> cursor2) const {
            while (!cursor1.done()) {
                auto bundle = cursor1.bundle(manager);
                pairsIntInner(fn, bundle, cursor2);
                cursor1.next();
            }

            for (auto i : detail::CursorChildren(cursor1)) {
                pairsIntDouble(fn, i, cursor2);
            }
        }

        void pairsInt (QueryPairCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                auto bundle = cursor.bundle(manager);
                auto next = cursor;

                next.next();
                pairsIntInner(fn, bundle, next);

                cursor = next;
            }

            auto children = detail::CursorChildren(cursor);
            for (auto it1 = children.begin(); it1 != children.end(); ++it1) {
                pairsInt(fn, *it1);
                auto it2 = it1;
                ++it2;
                for ( ; it2 != children.end(); ++it2) {
                    pairsIntDouble(fn, *it1, *it2);
                }
            }
        }

        friend detail::QueryableManager;
    public:
        Query () : manager{nullptr}, querySets{} {}

        explicit operator bool () const {
            return manager;
        }

        void entity (QueryCallback<Args...> auto fn, Entity entity) {
            PHENYL_DASSERT(*this);
            querySets.single(fn, entity);
        }

        void entity (QueryCallback<const Args...> auto fn, Entity entity) const {
            PHENYL_DASSERT(*this);
            manager->_defer();
            querySets.single(fn, entity);
            manager->_deferEnd();
        }

        void each (QueryCallback<Args...> auto fn) {
            PHENYL_DASSERT(*this);
            manager->_defer();
            eachInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }

        void each (QueryCallback<const Args...> auto fn) const {
            PHENYL_DASSERT(*this);
            manager->_defer();
            eachInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }

        void pairs (QueryPairCallback<Args...> auto fn) {
            PHENYL_DASSERT(*this);
            manager->_defer();
            pairsInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }

        void pairs (QueryPairCallback<const Args...> auto fn) const {
            PHENYL_DASSERT(*this);
            manager->_defer();
            pairsInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }
    };

    template <typename ...Args>
    class ConstQuery {
    private:
        detail::BasicManager* manager;
        detail::QuerySets<Args...> querySets;

        template <std::same_as<detail::ComponentSet> ...Args2>
        ConstQuery (const detail::BasicManager* manager, Args2*... args) requires (sizeof...(Args) == sizeof...(Args2)) : manager{const_cast<detail::BasicManager*>(manager)}, querySets{args...} {
            PHENYL_DASSERT(manager);
        }

        void eachInt (QueryCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                cursor.apply(fn, manager);
                cursor.next();
            }

            for (auto i : detail::CursorChildren(cursor)) {
                eachInt(fn, i);
            }
        }

        void pairsIntInner (QueryPairCallback<Args...> auto& fn, const QueryBundle<Args...>& bundle1, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                auto bundle2 = cursor.bundle(manager);
                fn(bundle1, bundle2);
                cursor.next();
            }

            for (auto i : detail::CursorChildren(cursor)) {
                pairsIntInner(fn, bundle1, i);
            }
        }

        void pairsIntDouble (QueryPairCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor1, detail::QueryCursor<Args...> cursor2) const {
            while (!cursor1.done()) {
                auto bundle = cursor1.bundle(manager);
                pairsIntInner(fn, bundle, cursor2);
                cursor1.next();
            }

            for (auto i : detail::CursorChildren(cursor1)) {
                pairsIntDouble(fn, i, cursor2);
            }
        }

        void pairsInt (QueryPairCallback<Args...> auto& fn, detail::QueryCursor<Args...> cursor) const {
            while (!cursor.done()) {
                auto bundle = cursor.bundle(manager);
                auto next = cursor;

                next.next();
                pairsIntInner(fn, bundle, next);

                cursor = next;
            }

            auto children = detail::CursorChildren(cursor);
            for (auto it1 = children.begin(); it1 != children.end(); ++it1) {
                pairsInt(fn, *it1);
                auto it2 = it1;
                ++it2;
                for ( ; it2 != children.end(); ++it2) {
                    pairsIntDouble(fn, *it1, *it2);
                }
            }
        }

        friend detail::QueryableManager;
    public:
        ConstQuery () : manager{nullptr}, querySets{} {}

        explicit operator bool () const {
            return manager;
        }

        void entity (ConstQueryCallback<Args...> auto fn, Entity entity) const {
            PHENYL_DASSERT(*this);
            querySets.single(fn, entity);
        }

        void each (ConstQueryCallback<Args...> auto fn) const {
            PHENYL_DASSERT(*this);
            manager->_defer();
            eachInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }

        void pairs (ConstQueryPairCallback<Args...> auto fn) const {
            PHENYL_DASSERT(*this);
            manager->_defer();
            pairsInt(fn, detail::QueryCursor<Args...>{querySets.getCursorRef()});
            manager->_deferEnd();
        }
    };
}