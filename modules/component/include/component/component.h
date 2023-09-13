#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <functional>
#include <iterator>

#include "forward.h"
#include "entity_id.h"
#include "detail/component_set.h"
#include "detail/typed_set.h"
#include "detail/entity_id_list.h"
#include "detail/prefab_list.h"
#include "component/detail/signals/signal_handler.h"
#include "detail/component_utils.h"
#include "component/detail/managers/basic_manager.h"
#include "component/detail/managers/query_manager.h"
#include "detail/children_view.h"
#include "query.h"

#include "component/entity.h"

#include "util/map.h"
#include "util/meta.h"
#include "util/optional.h"

namespace component {
    class Prefab;
    class PrefabBuilder;

    class ComponentManager : protected detail::QueryableManager {
    private:
        class EntityIterator {
        private:
            detail::EntityIdList::const_iterator it;
            ComponentManager* compManager;
            EntityIterator (ComponentManager* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = Entity;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            EntityIterator () : compManager{nullptr}, it{} {};

            value_type operator* () const {
                return compManager->entity(*it);
            }

            EntityIterator& operator++ () {
                ++it;
                return *this;
            }
            EntityIterator operator++ (int) {
                auto copy = *this;
                ++*this;

                return copy;
            }

            EntityIterator& operator-- () {
                --it;
                return *this;
            }
            EntityIterator operator-- (int) {
                auto copy = *this;
                --*this;

                return copy;
            }

            bool operator== (const EntityIterator& other) const {
                return it == other.it;
            }

            friend class ComponentManager;
        };

        class ConstEntityView {
        private:
            detail::EntityIdList::const_iterator it;
            const ComponentManager* compManager;
            ConstEntityView (const ComponentManager* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = ConstEntity;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            ConstEntityView () : compManager{nullptr}, it{} {};

            value_type operator* () const {
                return compManager->entity(*it);
            }

            ConstEntityView& operator++ () {
                ++it;
                return *this;
            }
            ConstEntityView operator++ (int) {
                auto copy = *this;
                ++*this;

                return copy;
            }

            ConstEntityView& operator-- () {
                --it;
                return *this;
            }
            ConstEntityView operator-- (int) {
                auto copy = *this;
                --*this;

                return copy;
            }

            bool operator== (const ConstEntityView& other) const {
                return it == other.it;
            }

            friend class ComponentManager;
        };

        std::size_t currStartCapacity;
        detail::PrefabList prefabs;

        template <typename T>
        std::unique_ptr<detail::ComponentSet> createComponent () requires (!std::is_abstract_v<T>) {
            auto component = std::make_unique<detail::ConcreteComponentSet<T>>(static_cast<detail::BasicManager*>(this), currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            if (deferCount) {
                component->defer();
            }

            return component;
        }

        template <typename T>
        std::unique_ptr<detail::ComponentSet> createComponent () requires (std::is_abstract_v<T>) {
            auto component = std::make_unique<detail::AbstractComponentSet<T>>(static_cast<detail::BasicManager*>(this), currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            if (deferCount) {
                component->defer();
            }

            return component;
        }

        template <typename T>
        detail::SignalHandlerList<T>* getOrCreateHandlerList () {
            auto typeIndex = meta::type_index<T>();
            if (signalHandlers.contains(typeIndex)) {
                return (detail::SignalHandlerList<T>*)signalHandlers[typeIndex].get();
            }

            auto handlerList = std::make_unique<detail::SignalHandlerList<T>>();
            detail::SignalHandlerList<T>* ptr = handlerList.get();
            signalHandlers[typeIndex] = std::move(handlerList);

            if (deferCount || signalDeferCount) {
                ptr->defer();
            }

            return ptr;
        }

        friend class PrefabBuilder;
        friend class Prefab;
        friend class ChildrenView;
    public:
        using iterator = EntityIterator;
        using const_iterator = ConstEntityView;

        static constexpr std::size_t START_CAPACITY = 256;
        static_assert(std::bidirectional_iterator<iterator>);
        static_assert(std::bidirectional_iterator<const_iterator>);

        explicit ComponentManager (std::size_t startCapacity=START_CAPACITY) : detail::QueryableManager{startCapacity}, currStartCapacity{startCapacity} {}

        ComponentManager (const ComponentManager&) = delete;
        ComponentManager (ComponentManager&&) = default;

        ComponentManager& operator= (const ComponentManager&) = delete;
        ComponentManager& operator= (ComponentManager&&) = default;

        ~ComponentManager () {
            clearAll();
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

        Entity create () {
            return entity(_create(EntityId{}));
        }

        ChildrenView root () {
            return _children(EntityId{});
        }

        [[nodiscard]] std::size_t size () const {
            return idList.size();
        }

        void clear () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            for (auto [i, comp] : components.kv()) {
                comp->clear();
            }
            idList.clear();
            relationships.reset();
        }

        void clearAll () {
            logging::log(LEVEL_DEBUG, "Clearing all!");
            clear();
            prefabs.clear();
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

        template <typename Signal, typename ...Args, meta::callable<void, const Signal&, Entity, std::remove_reference_t<Args>&...> F>
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
        void handleSignal (std::function<void(Entity, const Signal&)> handler) {
            auto* comp = (detail::TypedComponentSet<typename Signal::Type>*)getComponent<typename Signal::Type>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Failed to get component for component signal handler!");
                return;
            }

            comp->addHandler(handler);
        }

        void defer () {
            _defer();
        }

        void deferSignals () {
            _deferSignals();
        }

        void deferEnd () {
            _deferEnd();
        }

        void deferSignalsEnd () {
            _deferSignalsEnd();
        }


        // TODO: merge Entity/ConstEntity and EntityComponentView/ConstEntityComponentView
        Entity entity (EntityId id) {
            return _entity(id);
        }

        [[nodiscard]] ConstEntity entity (EntityId id) const {
            return _entity(id);
        }

        template <typename ...Args>
        Query<Args...> query () {
            return _query<Args...>();
        }

        template <typename ...Args>
        ConstQuery<Args...> query () const {
            return _query<Args...>();
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

    using EntityComponentManager = ComponentManager;
}