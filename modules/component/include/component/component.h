#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <functional>
#include <iterator>

#include "forward.h"
#include "entity_id.h"
#include "detail/component_set.h"
#include "detail/entity_id_list.h"
#include "detail/prefab_list.h"
#include "detail/signal_handler.h"
#include "detail/utils.h"

#include "util/map.h"
#include "util/meta.h"
#include "util/optional.h"

namespace component {
    namespace detail {
        struct Relationship {
            EntityId parent{};
            EntityId children{};
            EntityId next{};
            EntityId prev{};

            void clear () {
                parent = {};
                children = {};
                next = {};
                prev = {};
            }
        };
    }

    class Prefab;
    class PrefabBuilder;

    class ComponentManager {
    private:
        class ChildrenView;
        class EntityView {
        private:
            EntityId entityId;
            ComponentManager& compManager;
            EntityView (EntityId id, ComponentManager& compManager) : entityId{id}, compManager{compManager} {}
        public:
            EntityView () : entityId{}, compManager{compManager} {}

            [[nodiscard]] EntityId id () const {
                return entityId;
            }

            ComponentManager& manager () {
                return compManager;
            }

            const ComponentManager& manager () const {
                return compManager;
            }

            template <typename T>
            util::Optional<T&> get () {
                return compManager.get<T>(entityId);
            };

            template <typename T, typename ...Args>
            void insert (Args&&... args) {
                compManager.insert<T>(entityId, std::forward<Args>(args)...);
            }

            template <typename T>
            bool set (T comp) {
                return compManager.set(entityId, std::move(comp));
            }

            template <typename T>
            void erase () {
                compManager.erase<T>(entityId);
            }

            template <typename T>
            [[nodiscard]] bool has () const {
                return compManager.has<T>(entityId);
            }

            template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F>
            void apply (F fn) {
                compManager.apply<Args...>(fn, entityId);
            }

            void remove () {
                compManager.remove(entityId);
            }

            EntityView createChild () {
                return compManager.create(entityId);
            }

            ChildrenView children () {
                return compManager.children(entityId);
            }

            void reparent (EntityId parent) {
                return compManager.reparent(entityId, parent);
            }

            explicit operator bool () const {
                return (bool)entityId;
            }

            friend class ComponentManager;
        };

        class ConstEntityView {
        private:
            EntityId entityId;
            const ComponentManager& compManager;
            ConstEntityView (EntityId id, const ComponentManager& compManager) : entityId{id}, compManager{compManager} {}
        public:
            [[nodiscard]] EntityId id () const {
                return entityId;
            }

            template <typename T>
            util::Optional<const T&> get () const {
                return compManager.getComponent<T>(entityId);
            };

            template <typename T>
            [[nodiscard]] bool has () const {
                return compManager.has<T>(entityId);
            }

            friend class ComponentManager;
        };

        class EntityViewIterator {
        private:
            detail::EntityIdList::const_iterator it;
            ComponentManager* compManager;
            EntityViewIterator (ComponentManager* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = ComponentManager::EntityView;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            EntityViewIterator () : compManager{nullptr}, it{} {};

            value_type operator* () const {
                return compManager->view(*it);
            }

            EntityViewIterator& operator++ () {
                ++it;
                return *this;
            }
            EntityViewIterator operator++ (int) {
                auto copy = *this;
                ++*this;

                return copy;
            }

            EntityViewIterator& operator-- () {
                --it;
                return *this;
            }
            EntityViewIterator operator-- (int) {
                auto copy = *this;
                --*this;

                return copy;
            }

            bool operator== (const EntityViewIterator& other) const {
                return it == other.it;
            }

            friend class ComponentManager;
        };

        class ConstEntityViewIterator {
        private:
            detail::EntityIdList::const_iterator it;
            const ComponentManager* compManager;
            ConstEntityViewIterator (const ComponentManager* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = ComponentManager::ConstEntityView;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            ConstEntityViewIterator () : compManager{nullptr}, it{} {};

            value_type operator* () const {
                return compManager->view(*it);
            }

            ConstEntityViewIterator& operator++ () {
                ++it;
                return *this;
            }
            ConstEntityViewIterator operator++ (int) {
                auto copy = *this;
                ++*this;

                return copy;
            }

            ConstEntityViewIterator& operator-- () {
                --it;
                return *this;
            }
            ConstEntityViewIterator operator-- (int) {
                auto copy = *this;
                --*this;

                return copy;
            }

            bool operator== (const ConstEntityViewIterator& other) const {
                return it == other.it;
            }

            friend class ComponentManager;
        };

        class ChildrenView {
        private:
            class Iterator {
            private:
                EntityId curr;
                ComponentManager* manager;

                Iterator (ComponentManager* manager, EntityId curr) : manager{manager}, curr{curr} {}
                friend ChildrenView;
            public:
                using value_type = EntityView;
                using difference_type = std::ptrdiff_t;
                Iterator () : curr{}, manager{nullptr} {}

                value_type operator* () const {
                    return manager->view(curr);
                }

                Iterator& operator++ () {
                    curr = manager->getRelationship(curr).next;
                    return *this;
                }

                Iterator operator++ (int) {
                    Iterator it = *this;
                    ++*this;
                    return it;
                }

                bool operator== (const Iterator& other) const {
                    return manager == other.manager && curr == other.curr;
                }
            };

            class ConstIterator {
            private:
                EntityId curr;
                const ComponentManager* manager;

                ConstIterator (const ComponentManager* manager, EntityId curr) : manager{manager}, curr{curr} {}

                friend ChildrenView;
            public:
                using value_type = ConstEntityView;
                using difference_type = std::ptrdiff_t;
                ConstIterator () : curr{}, manager{nullptr} {}

                value_type operator* () const {
                    return manager->view(curr);
                }

                ConstIterator& operator++ () {
                    curr = manager->getRelationship(curr).next;
                    return *this;
                }

                ConstIterator operator++ (int) {
                    ConstIterator it = *this;
                    ++*this;
                    return it;
                }

                bool operator== (const ConstIterator& other) const {
                    return manager == other.manager && curr == other.curr;
                }
            };

            EntityId parentId;
            ComponentManager* manager;

            template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F, std::size_t ...Indexes>
            void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) {
                auto curr = manager->getRelationship(parentId).children;

                while (curr) {
                    auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(curr)...);
                    if (detail::tupleAllNonNull<Args...>(tup)) {
                        IterInfo info{manager, curr};
                        fn(info, *std::get<Indexes>(tup)...);
                    }

                    curr = manager->getRelationship(curr).next;
                }
            }

            template <typename ...Args, meta::callable<void, const IterInfo&, const Args&...> F, std::size_t ...Indexes>
            void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, F& fn, std::index_sequence<Indexes...>) const {
                auto curr = manager->getRelationship(parentId).children;

                while (curr) {
                    auto tup = std::make_tuple((Args*)compSets[Indexes]->getComponentUntyped(curr)...);
                    if (detail::tupleAllNonNull<Args...>(tup)) {
                        IterInfo info{manager, curr};
                        fn(info, *std::get<Indexes>(tup)...);
                    }

                    curr = manager->getRelationship(curr).next;
                }
            }

            ChildrenView (ComponentManager* manager, EntityId parent) : manager{manager}, parentId{parent} {}
            friend ComponentManager;
        public:
            using iterator = Iterator;
            using const_iterator = ConstIterator;
            static_assert(std::forward_iterator<iterator>);
            static_assert(std::forward_iterator<const_iterator>);

            iterator begin () {
                return iterator{manager, manager->getRelationship(parentId).children};
            }
            iterator end () {
                return iterator {manager, {}};
            }

            [[nodiscard]] const_iterator begin () const {
                return cbegin();
            }
            [[nodiscard]] const_iterator cbegin () const {
                return const_iterator{manager, manager->getRelationship(parentId).children};
            }

            [[nodiscard]] const_iterator end () const {
                return cend();
            }
            [[nodiscard]] const_iterator cend () const {
                return const_iterator{manager, {}};
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

            EntityView parent () {
                return manager->view(parentId);
            }

            [[nodiscard]] ConstEntityView parent () const {
                return ((const ComponentManager*)manager)->view(parentId);
            }
        };

        detail::EntityIdList idList;
        util::Map<std::size_t, std::unique_ptr<detail::ComponentSet>> components;
        util::Map<std::size_t, std::unique_ptr<detail::SignalHandlerListBase>> signalHandlers;
        std::size_t currStartCapacity;
        std::size_t deferCount{0};
        std::vector<EntityId> deferredDeletions{};
        std::vector<std::pair<std::function<void(ComponentManager*, EntityId)>, EntityId>> deferredApplys;
        std::vector<detail::Relationship> relationships{};
        detail::PrefabList prefabs;

        template <typename T>
        detail::ComponentSet* getComponent () const {
            auto typeIndex = meta::type_index<T>();
            if (!components.contains(typeIndex)) {
                logging::log(LEVEL_ERROR, "Failed to get component for index {}!", typeIndex);
                return nullptr;
            }

            return components.at(typeIndex).get();
        }

        template <typename T>
        std::unique_ptr<detail::ComponentSet> createComponent () requires (!std::is_abstract_v<T>) {
            auto component = std::make_unique<detail::ConcreteComponentSet<T>>(this, currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            return component;
        }

        template <typename T>
        std::unique_ptr<detail::ComponentSet> createComponent () requires (std::is_abstract_v<T>) {
            auto component = std::make_unique<detail::AbstractComponentSet<T>>(this, currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            return component;
        }

        /*template <typename T>
        detail::ComponentSet* getOrCreateComponent () {
            auto typeIndex = meta::type_index<T>();
            if (components.contains(typeIndex)) {
                return components[typeIndex].get();
            }

            auto component = createComponent<T>();
            auto* compPtr = component.get();

            components.emplace(typeIndex, std::move(component));

            return compPtr;
        }*/

        template <typename T>
        T* getEntityComp (EntityId id) const {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to get component from invalid entity {}!", id.value());
                return nullptr;
            }
            detail::ComponentSet* component = getComponent<T>();

            return component ? component->getComponent<T>(id) : nullptr;
        }

        template <typename T, meta::callable<void, IterInfo&, T&> F>
        void eachInt (detail::ComponentSet* componentSet, F& fn) {
            assert(componentSet);

            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = 0; i < componentSet->dataSize; i++) {
                T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                auto info = IterInfo{this, id};
                fn(info, comp);
            }

            auto* curr = componentSet->children;
            while (curr) {
                eachInt<T>(curr, fn);
                curr = curr->nextChild;
            }
        }

        template <typename T, meta::callable<void, const IterInfo&, const T&> F>
        void eachInt (detail::ComponentSet* componentSet, F& fn) const {
            assert(componentSet);

            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = 0; i < componentSet->dataSize; i++) {
                T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                const auto info = IterInfo{const_cast<ComponentManager*>(this), id};
                fn(info, comp);
            }

            auto* curr = componentSet->children;
            while (curr) {
                eachInt<T>(curr, fn);
                curr = curr->nextChild;
            }
        }

        template <typename ...Args, std::size_t ...Indexes, meta::callable<void, IterInfo&, std::remove_reference_t<Args>&...> F>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, detail::ComponentSet* primarySet, F& fn, std::index_sequence<Indexes...> indexes) {
            assert(primarySet);
            assert((std::get<Indexes>(compSets) && ...));

            for (std::size_t i = 0; i < primarySet->dataSize; i++) {
                auto id = primarySet->ids[i];
                auto tup = std::make_tuple((Args*)(std::get<Indexes>(compSets)->getComponentUntyped(id))...);

                if (detail::tupleAllNonNull<Args...>(tup)) {
                    IterInfo info{this, id};
                    fn(info, *std::get<Indexes>(tup)...);
                }
            }

            auto* curr = primarySet->children;
            while (curr) {
                eachInt<Args...>(compSets, curr, fn, indexes);
                curr = curr->nextChild;
            }
        }

        template <typename ...Args, std::size_t ...Indexes, meta::callable<void, const IterInfo&, std::remove_reference_t<Args>&...> F>
        void eachInt (std::array<detail::ComponentSet*, sizeof...(Args)>& compSets, detail::ComponentSet* primarySet, F& fn, std::index_sequence<Indexes...> indexes) const {
            static_assert(sizeof...(Args) == sizeof...(Indexes));
            assert(primarySet);
            assert((compSets[Indexes] && ...));

            for (std::size_t i = 0; i < primarySet->dataSize; i++) {
                auto id = primarySet->ids[i];
                auto tup = std::make_tuple((Args*)(compSets[Indexes]->getComponentUntyped(id))...);

                if (detail::tupleAllNonNull<Args...>(tup)) {
                    const IterInfo info{const_cast<ComponentManager*>(this), id};
                    fn(info, *std::get<Indexes>(tup)...);
                }
            }

            auto* curr = primarySet->children;
            while (curr) {
                eachInt<Args...>(compSets, curr, fn, indexes);
                curr = curr->nextChild;
            }
        }

        template <typename T, meta::callable<void, const std::tuple<IterInfo&, T&>&, const std::tuple<IterInfo&, T&>&> F>
        void eachPairIt (detail::ComponentSet* componentSet, std::size_t start, const std::tuple<IterInfo&, T&>& firstTup, F& fn) {
            assert(componentSet);
            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = start; i < componentSet->dataSize; i++) {
                T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                auto info = IterInfo{const_cast<ComponentManager*>(this), id};

                std::tuple<IterInfo&, T&> tup{info, comp};

                fn(firstTup, tup);
            }

            for (auto* curr = componentSet->children; curr; curr = curr->nextChild) {
                eachPairIt<T>(curr, 0, firstTup, fn);
            }
        }

        template <typename T, meta::callable<void, const std::tuple<IterInfo&, T&>&, const std::tuple<IterInfo&, T&>&> F>
        void eachPairDoubleIt (detail::ComponentSet* firstSet, detail::ComponentSet* secondSet, F& fn) {
            assert(firstSet);
            assert(secondSet);
            auto compSize = firstSet->compSize;
            auto dataPtr = firstSet->data.get();
            for (std::size_t i = 0; i < firstSet->dataSize; i++) {
                T& comp = *(T*)(dataPtr + i * compSize);
                auto id = firstSet->ids[i];
                auto info = IterInfo{const_cast<ComponentManager*>(this), id};

                std::tuple<IterInfo&, T&> tup{info, comp};
                eachPairIt<T>(secondSet, 0, tup, fn);
            }

            for (auto* curr = firstSet->children; curr; curr = curr->nextChild) {
                eachPairDoubleIt<T>(curr, secondSet, fn);
            }
        }

        template <typename T, meta::callable<void, const std::tuple<IterInfo&, T&>&, const std::tuple<IterInfo&, T&>&> F>
        void eachPairInt (detail::ComponentSet* componentSet, F& fn) {
            assert(componentSet);
            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = 0; i < componentSet->dataSize; i++) {
                T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                auto info = IterInfo{const_cast<ComponentManager*>(this), id};

                std::tuple<IterInfo&, T&> tup{info, comp};
                eachPairIt<T>(componentSet, i + 1, tup, fn);
            }

            for (auto* curr = componentSet->children; curr; curr = curr->nextChild) {
                eachPairInt<T>(curr, fn);
                for (auto* curr2 = curr->nextChild; curr2; curr2 = curr2->nextChild) {
                    eachPairDoubleIt<T>(curr, curr2, fn);
                }
            }
        }

        template <typename T, meta::callable<void, const std::tuple<const IterInfo&, const T&>&, const std::tuple<const IterInfo&, const T&>&> F>
        void eachPairIt (detail::ComponentSet* componentSet, std::size_t start, const std::tuple<const IterInfo&, const T&>& firstTup, F& fn) const {
            assert(componentSet);
            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = start; i < componentSet->dataSize; i++) {
                const T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                const auto info = IterInfo{const_cast<ComponentManager*>(this), id};

                fn(firstTup, std::make_tuple((const IterInfo&)info, comp));
            }

            for (auto* curr = componentSet->children; curr; curr = curr->nextChild) {
                eachPairIt<T>(curr, 0, firstTup, fn);
            }
        }

        template <typename T, meta::callable<void, const std::tuple<const IterInfo&, const T&>&, const std::tuple<const IterInfo&, const T&>&> F>
        void eachPairInt (detail::ComponentSet* componentSet, F& fn) const {
            assert(componentSet);
            auto compSize = componentSet->compSize;
            auto dataPtr = componentSet->data.get();
            for (std::size_t i = 0; i < componentSet->dataSize; i++) {
                const T& comp = *(T*)(dataPtr + i * compSize);
                auto id = componentSet->ids[i];
                const auto info = IterInfo{const_cast<ComponentManager*>(this), id};
                eachPairIt<T>(componentSet, i + 1, std::make_tuple((const IterInfo&)info, comp), fn);
            }

            for (auto* curr = componentSet->children; curr; curr = curr->nextChild) {
                eachPairInt<T>(curr, fn);
            }
        }

        template <typename T>
        detail::SignalHandlerList<T>* getOrCreateHandlerList () {
            auto typeIndex = meta::type_index<T>();
            if (signalHandlers.contains(typeIndex)) {
                return (detail::SignalHandlerList<T>*)signalHandlers[typeIndex].get();
            }

            auto handlerList = std::make_unique<detail::SignalHandlerList<T>>();
            auto* ptr = handlerList.get();
            signalHandlers[typeIndex] = std::move(handlerList);

            return ptr;
        }

        void onDeferBegin () {
            for (auto [k, v] : signalHandlers.kv()) {
                v->defer();
            }

            for (auto [k, v] : components.kv()) {
                v->defer();
            }
        }

        void onDeferEnd () {
            for (auto [k, v] : components.kv()) {
                v->deferEnd();
            }

            for (auto [k, v] : signalHandlers.kv()) {
                v->deferEnd(this);
            }

            for (auto& [f, id] : deferredApplys) {
                f(this, id);
            }

            for (auto i : deferredDeletions) {
                remove(i);
            }

            deferredApplys.clear();
            deferredDeletions.clear();
        }

        template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F>
        inline void applyNow (F& fn, EntityId id) {
            std::tuple<Args*...> compTup{getEntityComp<Args>(id)...};
            if (detail::tupleAllNonNull<Args...>(compTup)) {
                IterInfo info{this, id};
                fn(info, (*std::get<Args*>(compTup))...);
            }
        }

        detail::Relationship& getRelationship (EntityId id) {
            assert(id.id < relationships.size());

            return relationships[id.id];
        }

        [[nodiscard]] const detail::Relationship& getRelationship (EntityId id) const {
            assert(id.id < relationships.size());

            return relationships[id.id];
        }

        void setParent (EntityId id, EntityId parent) {
            getRelationship(id).parent = parent;
            auto oldStart = getRelationship(parent).children;

            getRelationship(id).next = oldStart;
            getRelationship(oldStart).prev = id;
            getRelationship(parent).children = id;
        }

        void removeFromParent (EntityId id) {
            auto& rel = getRelationship(id);

            if (rel.prev) {
                getRelationship(rel.prev).next = rel.next;
            } else {
                getRelationship(rel.parent).children = rel.next;
            }

            if (rel.next) {
                getRelationship(rel.next).prev = rel.prev;
            }
        }

        void removeRelationships (EntityId id, bool updateParent) {
            auto& rel = getRelationship(id);

            auto curr = rel.children;
            while (curr) {
                auto next = getRelationship(curr).next;
                removeInt(curr, false);
                curr = next;
            }

            if (updateParent) {
                removeFromParent(id);
            }

            rel.clear();
        }

        void removeInt (EntityId id, bool updateParent) {
            removeRelationships(id, updateParent);

            for (auto [i, comp] : components.kv()) {
                comp->deleteComp(id);
            }

            idList.removeId(id);
        }

        friend class PrefabBuilder;
        friend class Prefab;
    public:
        using iterator = EntityViewIterator;
        using const_iterator = ConstEntityViewIterator;
        using View = ComponentManager::EntityView;
        using ConstView = ComponentManager::ConstEntityView;
        using ChildrenView = ComponentManager::ChildrenView;

        template <typename ...Args>
        using Bundle = const std::tuple<IterInfo&, std::remove_reference_t<Args>&...>&;
        template <typename ...Args>
        using ConstBundle = const std::tuple<const IterInfo&, const std::remove_cvref_t<Args>&...>&;

        static constexpr std::size_t START_CAPACITY = 256;
        static_assert(std::bidirectional_iterator<iterator>);
        static_assert(std::bidirectional_iterator<const_iterator>);

        explicit ComponentManager (std::size_t startCapacity=START_CAPACITY) : idList{startCapacity}, components{}, currStartCapacity{startCapacity} {
            relationships.reserve(startCapacity);
            relationships.push_back(detail::Relationship{});
        }

        ComponentManager (const ComponentManager&) = delete;
        ComponentManager (ComponentManager&&) = default;

        ComponentManager& operator= (const ComponentManager&) = delete;
        ComponentManager& operator= (ComponentManager&&) = default;

        ~ComponentManager () {
            clear();
        }

        template <typename T>
        void addComponent () {
            auto typeIndex = meta::type_index<T>();
            if (components.contains(typeIndex)) {
                logging::log(LEVEL_ERROR, "Attempted to insert component type of index {} that already exists!", typeIndex);
                return;
            }

            auto component = createComponent<T>();

            components.emplace(typeIndex, std::move(component));
        }

        template <typename T>
        util::Optional<T&> get (EntityId id) {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<T&>{*comp} : util::Optional<T&>{};
        }

        template <typename T>
        util::Optional<const T&> get (EntityId id) const {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<const T&>{*comp} : util::Optional<const T&>{};
        }

        template <typename T, typename ...Args>
        void insert (EntityId id, Args&&... args) requires std::constructible_from<T, Args...> {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to insert component to invalid entity {}!", id.value());
                return;
            }
            //detail::ComponentSet* comp = getOrCreateComponent<T>();
            detail::ComponentSet* comp = getComponent<T>();
            if (!comp) {
                return;
            }

            comp->insertComp<T>(id, std::forward<Args>(args)...);
        }

        template <typename T>
        bool set (EntityId id, T comp) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to set component of invalid entity {}!", id.value());
                return false;
            }
            detail::ComponentSet* compSet = getComponent<T>();
            if (!compSet) {
                return false;
            }

            return compSet->setComp(id, std::move(comp));
        }

        EntityView create (EntityId parent=EntityId{}) {
            auto id = idList.newId();

            for (auto [i, comp] : components.kv()) {
                comp->guaranteeEntityIndex(id.id);
            }

            assert(relationships.size() >= id.id);
            if (relationships.size() <= id.id) {
                relationships.push_back(detail::Relationship{});
            }

            setParent(id, parent);

            return EntityView{id, *this};
        }

        [[nodiscard]] bool exists (EntityId id) const {
            return idList.check(id);
        }

        void remove (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to delete invalid entity {}!", id.value());
                return;
            }

            if (deferCount > 0) {
                deferredDeletions.push_back(id);
                return;
            }

            removeInt(id, true);
        }

        template <typename T>
        void erase (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to remove component from invalid entity {}!", id.value());
                return;
            }
            detail::ComponentSet* comp = getComponent<T>();
            if (!comp) {
                return;
            }

            comp->deleteComp(id);
        }

        template <typename T>
        bool has (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to check component status for invalid entity {}!", id.value());
                return false;
            }
            detail::ComponentSet* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Attempted to check component status of component with index {} that doesn't exist!", meta::type_index<T>());
                return false;
            }

            return comp->hasComp(id);
        }

        template <typename ...Args, meta::callable<void, IterInfo&, Args&...> F>
        void apply (F fn, EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to apply on invalid entity {}!", id.value());
                return;
            }

            if (!deferCount) {
                applyNow<Args...>(fn, id);
                return;
            }

            auto applyFn = [fn=std::move(fn)] (ComponentManager* manager, EntityId id) {
                manager->applyNow<Args...>(fn, id);
            };
            deferredApplys.emplace_back(std::move(applyFn), id);
        }

        ChildrenView children (EntityId id) {
            return ChildrenView{this, id};
        }

        void reparent (EntityId id, EntityId parent) {
            removeFromParent(id);
            setParent(id, parent);
        }

        std::size_t size () const {
            return idList.size();
        }

        void clear () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            for (auto [i, comp] : components.kv()) {
                comp->clear();
            }

            idList.clear();
        }

        template <typename Derived, typename Base>
        void inherits () requires std::derived_from<Derived, Base> {
            detail::ComponentSet* derived = getComponent<Derived>();
            detail::ComponentSet* base = getComponent<Base>();

            if (!derived) {
                logging::log(LEVEL_ERROR, "Failed to get derived component!");
            }

            if (!base) {
                logging::log(LEVEL_ERROR, "Failed to get base component!");
            }

            if (!derived || !base) {
                return;
            }

            if (derived->setParent(base)) {
                base->addChild(derived);
            }
        }

        template <typename Dependent, typename Dependency>
        void addRequirement () {
            detail::ComponentSet* dependent = getComponent<Dependent>();
            detail::ComponentSet* dependency = getComponent<Dependency>();

            if (!dependent) {
                logging::log(LEVEL_ERROR, "Failed to get dependent component!");
            }

            if (!dependency) {
                logging::log(LEVEL_ERROR, "Failed to get dependency component!");
            }

            if (!dependent || !dependency) {
                return;
            }

            dependency->addDependent(dependent);
        }

        template <typename Signal, typename ...Args, meta::callable<void, const Signal&, IterInfo&, std::remove_reference_t<Args>&...> F>
        void handleSignal (F fn) requires (!ComponentSignal<Signal>) {
            auto comps = std::array{getComponent<std::remove_reference_t<Args>>()...};
            for (auto i : comps) {
                if (!i) {
                    logging::log(LEVEL_ERROR, "Failed to get all components for signal handler!");
                    return;
                }
            }

            detail::SignalHandlerList<Signal>* handlerList = getOrCreateHandlerList<Signal>();

            auto handler = std::make_unique<detail::TypedSignalHandler<Signal, F, std::remove_reference_t<Args>...>>(std::move(fn), std::move(comps));
            handlerList->addHandler(std::move(handler));
        }

        template <ComponentSignal Signal>
        void handleSignal (std::function<void(IterInfo&, const Signal&)> handler) {
            auto* comp = (detail::TypedComponentSet<typename Signal::Type>*)getComponent<typename Signal::Type>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Failed to get component for component signal handler!");
                return;
            }

            comp->addHandler(handler);
        }

        template <typename Signal, typename ...Args>
        void signal (EntityId id, Args&&... args) requires (!ComponentSignal<Signal>) {
            detail::SignalHandlerList<Signal>* handlerList = getOrCreateHandlerList<Signal>();

            handlerList->handle(id, this, std::forward<Args>(args)...);
        }

        void defer () {
            if (deferCount++ == 0) {
                onDeferBegin();
            }
        }

        void deferEnd () {
            if (--deferCount == 0) {
                onDeferEnd();
            }
        }


        // TODO: merge EntityView/ConstEntityView and EntityComponentView/ConstEntityComponentView
        EntityView view (EntityId id) {
            return EntityView{id, *this};
        }

        [[nodiscard]] ConstEntityView view (EntityId id) const {
            return ConstEntityView{id, *this};
        }

        template <typename ...Args, typename = std::enable_if_t<1 < sizeof...(Args)>>
        util::Optional<std::tuple<std::remove_reference_t<Args>&...>> get (EntityId entityId) {
            std::tuple<std::remove_cvref_t<Args>*...> ptrs{getEntityComp<std::remove_cvref_t<Args>>(entityId)...};

            if (detail::tupleAllNonNull<0, Args...>(ptrs)) {
                return util::Optional<std::tuple<std::remove_reference_t<Args>&...>>{{*std::get<std::remove_cvref_t<Args>*>(ptrs)...}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename ...Args, typename = std::enable_if_t<1 < sizeof...(Args)>>
        util::Optional<std::tuple<const std::remove_cvref_t<Args>&...>> get (EntityId entityId) const {
            std::tuple<const std::remove_cvref_t<Args>*...> ptrs{getEntityComp<std::remove_cvref_t<Args>>(entityId)...};

            if (detail::tupleAllNonNull<0, const Args...>(ptrs)) {
                return util::Optional<std::tuple<const std::remove_cvref_t<Args>&...>>{{*std::get<const std::remove_cvref_t<Args>*>>(ptrs)...}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename T, meta::callable<void, IterInfo&, std::remove_reference_t<T>&> F>
        void each (F fn) {
            defer();
            auto* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Attempted to iterate through component without it being added yet!");
                return;
            }
            eachInt<std::remove_reference_t<T>>(comp, fn);
            deferEnd();
        }

        template <typename T, meta::callable<void, const IterInfo&, const std::remove_cvref_t<T>&> F>
        void each (F fn) const {
            auto* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Attempted to iterate through component without it being added yet!");
                return;
            }

            eachInt<std::remove_cvref_t<T>>(comp, fn);
        }

        template <typename ...Args, meta::callable<void, IterInfo&, std::remove_reference_t<Args>&...> F>
        void each (F fn) requires (sizeof...(Args) > 1) {
            auto comps = std::array{getComponent<Args>()...};
            for (auto i : comps) {
                if (!i) {
                    logging::log(LEVEL_ERROR, "Failed to get all components for each!");
                    return;
                }
            }

            detail::ComponentSet* primarySet = nullptr;
            std::size_t minSize = std::numeric_limits<std::size_t>::max();

            for (detail::ComponentSet* i : comps) {
                if (i->size() < minSize) {
                    primarySet = i;
                    minSize = i->size();
                }
            }

            defer();
            eachInt<std::remove_reference_t<Args>...>(comps, primarySet, fn, std::make_index_sequence<sizeof...(Args)>{});
            deferEnd();
        }

        template <typename ...Args, meta::callable<void, const IterInfo&, const std::remove_cvref_t<Args>&...> F>
        void each (F fn) const requires (sizeof...(Args) > 1) {
            auto comps = std::array{getComponent<Args>()...};
            for (std::size_t i = 0; i < sizeof...(Args); i++) {
                if (!comps[i]) {
                    logging::log(LEVEL_ERROR, "Attempted to iterate through component without it being added yet!");
                    return;
                }
            }

            detail::ComponentSet* primarySet = nullptr;
            std::size_t minSize = std::numeric_limits<std::size_t>::max();

            for (detail::ComponentSet* i : comps) {
                if (i->size() < minSize) {
                    primarySet = i;
                    minSize = i->size();
                }
            }

            eachInt<std::remove_reference_t<Args>...>(comps, primarySet, fn, std::make_index_sequence<sizeof...(Args)>{});
        }

        template <typename T, meta::callable<void, Bundle<T>, Bundle<T>> F>
        void eachPair (F fn) {
            auto* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Failed to get component for each pair!");
                return;
            }

            defer();
            eachPairInt<std::remove_reference_t<T>>(comp, fn);
            deferEnd();
        }

        template <typename T, meta::callable<void, ConstBundle<T>, ConstBundle<T>> F>
        void eachPair (F fn) const {
            auto* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Failed to get component for each pair!");
                return;
            }

            eachPairInt<std::remove_cvref_t<T>>(comp, fn);
        }

        PrefabBuilder buildPrefab ();

        iterator begin () {
            return iterator{this, idList.begin()};
        }
        iterator end () {
            return iterator{this, idList.end()};
        }

        const_iterator begin () const {
            return cbegin();
        }
        const_iterator cbegin () const {
            return const_iterator{this, idList.begin()};
        }

        const_iterator end () const {
            return cend();
        }
        const_iterator cend () const {
            return const_iterator{this, idList.end()};
        }
    };

    using EntityView = ComponentManager::View;
    using ConstEntityView = ComponentManager::ConstView;

    using EntityComponentManager = ComponentManager;
}