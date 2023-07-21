#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

#include "forward.h"

#include "util/map.h"
#include "util/meta.h"
#include "util/smart_help.h"
#include "util/optional.h"

namespace component {
    namespace detail {
        class ComponentSet;
        class EntityIdList;
    }

    struct EntityId {
    private:
        unsigned int generation;
        unsigned int id;
    public:
        EntityId () = default;
        EntityId (unsigned int _generation, unsigned int _id) : generation(_generation), id(_id) {}

        [[nodiscard]] std::size_t value () const {
            return (static_cast<std::size_t>(generation) << 32) | id;
        }

        explicit operator bool () const {
            return id != 0;
        }

        friend class ComponentManager;
        friend class detail::ComponentSet;
        friend class detail::EntityIdList;
    };

    namespace detail {
        template <typename ...Args, std::size_t N = 0>
        bool tupleAllNonNull (const std::tuple<std::remove_reference_t<Args>*...>& tup) {
            if constexpr (N == sizeof...(Args)) {
                return true;
            } else {
                return tupleAllNonNull<Args..., N+1>(tup) && std::get<N>(tup);
            }
        }

        template <typename ...Args>
        class ComponentView;

        class ComponentSet {
        private:
            static constexpr std::size_t EMPTY_INDEX = -1;
            static constexpr std::size_t RESIZE_FACTOR = 2;

            std::vector<EntityId> ids;
            std::vector<std::size_t> indexSet;

            std::unique_ptr<std::byte[]> data;
            std::size_t compSize;
            std::size_t dataSize;
            std::size_t dataCapacity;

            template <typename T>
            inline void assertType () {
                // TODO: insert ifndef
                assertTypeIndex(meta::type_index<T>(), typeid(T).name());
            }

            std::byte* tryInsert (EntityId id);

            void guaranteeCapacity (std::size_t capacity);

            template <typename ...Args>
            friend class detail::ComponentView;
        protected:
            virtual void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const = 0;
            virtual void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) = 0;
            virtual void moveTypedComp (std::byte* dest, std::byte* src) = 0;
            virtual void deleteTypedComp (std::byte* comp) = 0;

        public:
            ComponentSet (std::size_t startCapacity, std::size_t compSize);
            virtual ~ComponentSet ();

            void guaranteeEntityIndex (std::size_t index);

            [[nodiscard]] std::byte* getComponentUntyped (EntityId id) const;

            template <typename T>
            T* getComponent (EntityId id) {
                assertType<T>();

                return (T*)getComponentUntyped(id);
            }

            template <typename T>
            const T* getComponent (EntityId id) const {
                assertType<T>();

                return (const T*)getComponentUntyped(id);
            }

            template <typename T, typename ...Args>
            T* insertComp (EntityId id, Args&&... args) {
                auto* comp = tryInsert(id);

                if (comp) {
                    new ((T*)comp) T(std::forward<Args>(args)...);

                    return (T*)comp;
                } else {
                    return nullptr;
                }
            }

            bool hasComp (EntityId id) const;

            void deleteComp (EntityId id);
            void clear ();
        };

        template <typename T>
        class ConcreteComponentSet : public ComponentSet {
        protected:
            void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const override {
                if (typeIndex != meta::type_index<T>()) {
                    component::logging::log(LEVEL_FATAL, "Attempted to access component of type index {} ({}) with type of index {} ()!", meta::type_index<T>(), typeid(T).name(), typeIndex, debugOtherName);
                    std::exit(1);
                }
            }

            void moveTypedComp (std::byte* dest, std::byte* src) override {
                *((T*) dest) = std::move(*((T*) src));

                ((T*) src)->~T();
            }

            void deleteTypedComp (std::byte* comp) override {
                ((T*) comp)->~T();
            }

            void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) override {
                T* destPtr = (T*)dest;
                T* srcPtr = (T*)src;

                for (std::size_t i = 0; i < len; i++) {
                    new (destPtr) T(std::move(*srcPtr));
                    srcPtr->~T();

                    destPtr++;
                    srcPtr++;
                }
            }
        public:
            explicit ConcreteComponentSet (std::size_t startCapacity) : ComponentSet(startCapacity, sizeof(T)) {}

            ~ConcreteComponentSet () override {
                clear();
            }
        };

        class EntityIdList {
        private:
            static constexpr std::size_t GEN_BITS = sizeof(unsigned int) * 8 - 1;
            static constexpr std::size_t NUM_GENS = std::size_t{1} << GEN_BITS;
            static constexpr std::size_t GEN_MASK = (std::size_t{1} << GEN_BITS) - 1;
            static constexpr std::size_t FREE_LIST_BITS = sizeof(std::size_t) * 8 - GEN_BITS - 1;
            static constexpr std::size_t FREE_LIST_MASK = ((std::size_t{1} << FREE_LIST_BITS) - 1) << GEN_BITS;
            static constexpr std::size_t FREE_LIST_EMPTY = 0;
            static constexpr std::size_t MAX_NUM_IDS = (std::size_t{1} << (sizeof(unsigned int) * 8)) - 1;
            static constexpr std::size_t EMPTY_BIT = std::size_t{1} << (sizeof(std::size_t) * 8 - 1);

            class IdIterator {
            private:
                const EntityIdList* idList;
                std::size_t slotPos;
                explicit IdIterator (const EntityIdList* idList, std::size_t slotPos);

                void next ();
                void prev ();
            public:
                using value_type = EntityId;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;
                IdIterator () : idList{nullptr}, slotPos{0} {}

                value_type operator* () const;

                IdIterator& operator++ ();
                IdIterator operator++ (int);

                IdIterator& operator-- ();
                IdIterator operator-- (int);

                bool operator== (const IdIterator& other) const;

                friend class EntityIdList;
            };

            std::vector<std::size_t> idSlots;
            std::size_t numEntities;
            std::size_t freeListStart;
        public:
            using const_iterator = IdIterator;
            using iterator = const_iterator;
            static_assert(std::bidirectional_iterator<iterator>);
            static_assert(std::bidirectional_iterator<const_iterator>);

            explicit EntityIdList (std::size_t capacity);

            EntityId newId ();
            [[nodiscard]] bool check (EntityId id) const;

            void removeId (EntityId id);
            void clear ();

            [[nodiscard]] std::size_t size () const;
            [[nodiscard]] std::size_t maxIndex () const;

            [[nodiscard]] iterator begin () const;
            [[nodiscard]] const_iterator cbegin () const;

            [[nodiscard]] iterator end () const;
            [[nodiscard]] const_iterator cend () const;
        };
    }

    /*template <typename ...Args>
    class EntityComponentView {
    private:
        std::tuple<std::remove_reference_t<Args>&...> comps;
        EntityId entityId;
        explicit EntityComponentView (EntityId entityId, std::tuple<std::remove_cvref_t<Args>&...> comps) : entityId{entityId}, comps{comps} {}

        template <typename T, std::size_t N, typename U, typename ...Args2>
        static constexpr std::size_t getTypePos () {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>) {
                return N;
            } else {
                return getTypePos<T, N+1, Args2...>();
            }
        }
    public:
        template <typename T>
        auto& get () {
            return std::get<getTypePos<T, 0, Args...>()>(comps);
        }

        template <typename T>
        const auto& get () const {
            return std::get<getTypePos<T, 0, Args...>()>(comps);
        }

        [[nodiscard]] EntityId id () const {
            return entityId;
        }

        template <typename ...Args2>
        EntityComponentView<Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>,
                          "All requested types must be accessible!");
            return EntityComponentView<Args2...>{entityId, std::get<std::remove_cvref_t<Args2>&>(comps)...};
        }

        friend class ComponentManager;
        friend class detail::ComponentView<Args...>;
    };*/

    namespace detail {
        template <typename ...Args>
        class IdComponentView;

        template <typename ...Args>
        class ComponentView {
        private:
            static constexpr std::size_t NUM_ARGS = sizeof...(Args);

            class ViewIterator {
            private:
                const std::array<ComponentSet*, NUM_ARGS>* comps{nullptr};
                ComponentSet* primarySet{nullptr};
                std::size_t pos{0};

                template <typename T>
                T& getCurrComp (ComponentSet* comp, EntityId id) const {
                    if (primarySet == comp) {
                        return *(T*)(primarySet->data.get() + pos * primarySet->compSize);
                    } else {
                        return *comp->getComponent<T>(id);
                    }
                }

                template <std::size_t... Indexes>
                std::tuple<Args&...> getCurrComps (EntityId id, std::integer_sequence<std::size_t, Indexes...>) const {
                    return {getCurrComp<Args>((*comps)[Indexes], id)...};
                }

                bool valid () const {
                    for (auto i : *comps) {
                        if (i == primarySet) {
                            continue;
                        } else if (!i->hasComp(currId())) {
                            return false;
                        }
                    }

                    return true;
                }

                EntityId currId () const {
                    return primarySet->ids[pos];
                }

                void next () {
                    while (++pos < primarySet->dataSize && !valid()) ;
                }

                void prev () {
                    while (--pos >= 0 && !valid()) ;
                }

                ViewIterator (const std::array<ComponentSet*, NUM_ARGS>* comps, ComponentSet* primarySet, std::size_t pos) : comps{comps}, primarySet{primarySet}, pos{pos} {
                    assert(comps);
                    assert(primarySet);
                    if (pos != primarySet->dataSize && !valid()) {
                        next();
                    }
                }
                friend class ComponentView<Args...>;
                friend class IdComponentView<Args...>;
            public:
                //using value_type = EntityComponentView<Args...>;
                using value_type = std::tuple<Args&...>;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                ViewIterator () = default;

                value_type operator* () const {
                    //return value_type{currId(), getCurrComps(currId(), std::make_index_sequence<NUM_ARGS>{})};
                    return getCurrComps(currId(), std::make_index_sequence<NUM_ARGS>{});
                }

                ViewIterator& operator++ () {
                    next();
                    return *this;
                }
                ViewIterator operator++ (int) {
                    auto copy = *this;
                    ++*this;
                    return copy;
                }

                ViewIterator& operator-- () {
                    prev();
                    return *this;
                }
                ViewIterator operator-- (int) {
                    auto copy = *this;
                    ++*this;
                    return copy;
                }

                bool operator== (const ViewIterator& other) const {
                    return comps == other.comps && pos == other.pos;
                }
            };

            std::array<ComponentSet*, NUM_ARGS> comps;
            ComponentSet* primarySet{nullptr};

            template <std::same_as<ComponentSet*> ...Args2>
            ComponentView (Args2... args) : comps{args...}, primarySet{nullptr} {
                static_assert(sizeof...(Args2) == NUM_ARGS, "Incorrect number of component sets passed!");

                std::size_t minSize = std::numeric_limits<std::size_t>::max();
                for (auto i : comps) {
                    assert(i);
                    if (i->indexSet.size() < minSize) {
                        minSize = i->indexSet.size();
                        primarySet = i;
                    }
                }
            }
            friend class component::ComponentManager;
        public:
            using iterator = ViewIterator;
            using const_iterator = iterator;

            iterator begin () const {
                return iterator{&comps, primarySet, 0};
            }
            const_iterator cbegin () const {
                return begin();
            }

            iterator end () const {
                return iterator{&comps, primarySet, primarySet->dataSize};
            }
            const_iterator cend () const {
                return end();
            }

            IdComponentView<Args...> withId ();
        };

        template <typename T>
        class ComponentView<T> {
        private:
            class ViewIterator {
            private:
                ComponentSet* component{nullptr};
                std::size_t pos{0};
                ViewIterator (ComponentSet* component, std::size_t pos) : component{component}, pos{pos} {
                    assert(component);
                }

                EntityId currId () const {
                    return component->ids[pos];
                }

                friend class ComponentView<T>;
                friend class IdComponentView<T>;
            public:
                using value_type = T;
                using reference = T&;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                ViewIterator () = default;

                reference operator* () const {
                    return ((T*)component->data.get())[pos];
                }

                ViewIterator& operator++ () {
                    pos++;

                    return *this;
                }
                ViewIterator operator++ (int) {
                    auto copy = *this;
                    ++*this;

                    return copy;
                }

                ViewIterator& operator-- () {
                    pos--;

                    return *this;
                }
                ViewIterator operator-- (int) {
                    auto copy = *this;
                    --*this;

                    return copy;
                }

                bool operator== (const ViewIterator& other) const {
                    return component == other.component && pos == other.pos;
                }
            };
            ComponentSet* component;
            ComponentView (ComponentSet* component) : component{component} {}

            friend class component::ComponentManager;
        public:
            using iterator = ViewIterator;
            using const_iterator = iterator;

            iterator begin () const {
                return iterator{component, 0};
            }
            const_iterator cbegin () const {
                return begin();
            }

            iterator end () const {
                return iterator{component, component->dataSize};
            }
            const_iterator cend () const {
                return end();
            }

            IdComponentView<T> withId ();
        };

        static_assert(std::bidirectional_iterator<ComponentView<int>::iterator>);

        template <typename ...Args>
        class IdComponentView {
        private:
            ComponentView<Args...> compView;

            static EntityId getId (const ComponentView<Args...>::iterator& it) {
                return it.currId();
            }

            class Iterator {
            private:
                ComponentView<Args...>::iterator it;

                Iterator (ComponentView<Args...>::iterator it) : it{it} {}
                friend class IdComponentView<Args...>;
            public:
                using value_type = std::tuple<EntityId, Args&...>;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                Iterator () = default;

                value_type operator* () const {
                    return std::tuple_cat(std::tuple{IdComponentView<Args...>::getId(it)}, *it);
                }

                Iterator& operator++ () {
                    ++it;
                    return *this;
                }
                Iterator operator++ (int) {
                    auto copy = *this;
                    ++*this;

                    return copy;
                }

                Iterator& operator-- () {
                    --it;

                    return *this;
                }
                Iterator operator-- (int) {
                    auto copy = *this;
                    --*this;

                    return copy;
                }

                bool operator== (const Iterator& other) const {
                    return it == other.it;
                }
            };


            friend class Iterator;
        public:
            using iterator = Iterator;
            using const_iterator = iterator;

            IdComponentView (ComponentView<Args...> compView) : compView{compView} {}

            iterator begin () const {
                return iterator{compView.begin()};
            }
            const_iterator cbegin () const {
                return const_iterator{compView.cbegin()};
            }

            iterator end () const {
                return iterator{compView.end()};
            }
            const_iterator cend () const {
                return const_iterator{compView.cend()};
            }
        };

        template <typename T>
        class IdComponentView<T> {
        private:
            ComponentView<T> compView;

            static EntityId getId (const ComponentView<T>::iterator& it) {
                return it.currId();
            }

            class Iterator {
            private:
                ComponentView<T>::iterator it;

                Iterator (ComponentView<T>::iterator it) : it{it} {}
                friend class IdComponentView<T>;
            public:
                using value_type = std::tuple<EntityId, T&>;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                Iterator () = default;

                value_type operator* () const {
                    return std::tuple<EntityId, T&>{IdComponentView<T>::getId(it), *it};
                }

                Iterator& operator++ () {
                    ++it;
                    return *this;
                }
                Iterator operator++ (int) {
                    auto copy = *this;
                    ++*this;

                    return copy;
                }

                Iterator& operator-- () {
                    --it;

                    return *this;
                }
                Iterator operator-- (int) {
                    auto copy = *this;
                    --*this;

                    return copy;
                }

                bool operator== (const Iterator& other) const {
                    return it == other.it;
                }
            };


            friend class Iterator;
        public:
            using iterator = Iterator;
            using const_iterator = iterator;

            IdComponentView (ComponentView<T> compView) : compView{compView} {}

            iterator begin () const {
                return iterator{compView.begin()};
            }
            const_iterator cbegin () const {
                return const_iterator{compView.cbegin()};
            }

            iterator end () const {
                return iterator{compView.end()};
            }
            const_iterator cend () const {
                return const_iterator{compView.cend()};
            }
        };

        template <typename ...Args>
        inline IdComponentView<Args...> ComponentView<Args...>::withId () {
            return IdComponentView<Args...>{*this};
        }

        template <typename T>
        inline IdComponentView<T> ComponentView<T>::withId () {
            return IdComponentView<T>{*this};
        }
    }

    class ComponentManager/* : public util::SmartHelper<ComponentManager, true>*/ {
    private:
        class EntityView {
        private:
            EntityId entityId;
            ComponentManager& compManager;
            EntityView (EntityId id, ComponentManager& compManager) : entityId{id}, compManager{compManager} {}
        public:
            [[nodiscard]] EntityId id () const {
                return entityId;
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
            void erase () {
                compManager.erase<T>(entityId);
            }

            template <typename T>
            [[nodiscard]] bool has () const {
                return compManager.has<T>(entityId);
            }

            void remove () {
                compManager.remove(entityId);
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

            EntityViewIterator () = default;

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

            ConstEntityViewIterator () = default;

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

        detail::EntityIdList idList;
        util::Map<std::size_t, std::unique_ptr<detail::ComponentSet>> components;
        std::size_t currStartCapacity;

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
        detail::ComponentSet* getOrCreateComponent () {
            auto typeIndex = meta::type_index<T>();
            if (components.contains(typeIndex)) {
                return components[typeIndex].get();
            }

            std::unique_ptr<detail::ComponentSet> component = std::make_unique<detail::ConcreteComponentSet<T>>(currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            auto* compPtr = component.get();

            components.emplace(typeIndex, std::move(component));

            return compPtr;
        }

        template <typename T>
        T* getEntityComp (EntityId id) const {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to get component from invalid entity {}!", id.value());
                return nullptr;
            }
            detail::ComponentSet* component = getComponent<T>();

            return component ? component->getComponent<T>(id) : nullptr;
        }
    public:
        using iterator = EntityViewIterator;
        using const_iterator = ConstEntityViewIterator;
        using View = ComponentManager::EntityView;
        using ConstView = ComponentManager::ConstEntityView;
        static_assert(std::bidirectional_iterator<iterator>);
        static_assert(std::bidirectional_iterator<const_iterator>);

        explicit ComponentManager (std::size_t startCapacity) : idList{startCapacity}, components{}, currStartCapacity{startCapacity} {}

        template <typename T>
        void addComponent () {
            auto typeIndex = meta::type_index<T>();
            if (components.contains(typeIndex)) {
                logging::log(LEVEL_ERROR, "Attempted to insert component type of index {} that already exists!", typeIndex);
                return;
            }

            std::unique_ptr<detail::ComponentSet> component = std::make_unique<detail::ConcreteComponentSet<T>>(currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

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
        void insert (EntityId id, Args&&... args) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to insert component to invalid entity {}!", id.value());
                return;
            }
            detail::ComponentSet* comp = getOrCreateComponent<T>();

            comp->insertComp<T>(id, std::forward<Args>(args)...);
        }

        EntityView create () {
            auto id = idList.newId();

            for (auto [i, comp] : components.kv()) {
                comp->guaranteeEntityIndex(id.id);
            }

            return EntityView{id, *this};
        }

        void remove (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to delete invalid entity {}!", id.value());
                return;
            }

            for (auto [i, comp] : components.kv()) {
                comp->deleteComp(id);
            }

            idList.removeId(id);
        }

        template <typename T>
        void erase (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to remove component from invalid entity {}!", id.value());
                return;
            }
            detail::ComponentSet* comp = getComponent<T>();
            assert(comp);

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

        std::size_t size () const {
            return idList.size();
        }

        void clearEntities () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            for (auto [i, comp] : components.kv()) {
                comp->clear();
            }

            idList.clear();
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

            if (detail::tupleAllNonNull(ptrs)) {
                return util::Optional{{*std::get<std::remove_cvref_t<Args>>(ptrs)...}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename ...Args, typename = std::enable_if_t<1 < sizeof...(Args)>>
        util::Optional<std::tuple<const std::remove_cvref_t<Args>&...>> get (EntityId entityId) const {
            std::tuple<const std::remove_cvref_t<Args>*...> ptrs{getEntityComp<std::remove_cvref_t<Args>>(entityId)...};

            if (detail::tupleAllNonNull(ptrs)) {
                return util::Optional{{*std::get<const std::remove_cvref_t<Args>*>>(ptrs)...}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename ...Args>
        detail::ComponentView<std::remove_reference_t<Args>...> iterate () {
            return detail::ComponentView<std::remove_reference_t<Args>...>{getOrCreateComponent<std::remove_cvref_t<Args>>()...};
        }

        template <typename ...Args>
        detail::ComponentView<const std::remove_cvref_t<Args>...> iterate () const {
            return detail::ComponentView<const std::remove_cvref_t<Args>...>{getComponent<std::remove_cvref_t<Args>>()...};
        }

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