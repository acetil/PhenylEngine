#pragma once

#include <cstddef>
#include <limits>

#include "component.h"

namespace component {
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
                // TODO: add ifndef
                assertTypeIndex(meta::type_index<T>());
            }

            std::byte* tryInsert (EntityId id);

            void guaranteeCapacity (std::size_t capacity);

            template <typename ...Args>
            friend class detail::ComponentView;
        protected:
            virtual void assertTypeIndex (std::size_t typeIndex) const = 0;
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
            void assertTypeIndex (std::size_t typeIndex) const override {
                if (typeIndex != meta::type_index<T>()) {
                    component::logging::log(LEVEL_FATAL, "Attempted to access component of type index {} with type of index {}!", meta::type_index<T>(), typeIndex);
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
                explicit IdIterator (const EntityIdList* idList, std::size_t slotPos) : idList{idList}, slotPos{slotPos} {
                    if (slotPos != idList->idSlots.size() && (idList->idSlots[slotPos] & EMPTY_BIT)) {
                        next();
                    }
                }

                void next () {
                    if (slotPos == idList->idSlots.size()) {
                        return;
                    }

                    while (++slotPos < idList->idSlots.size() && (idList->idSlots[slotPos] & EMPTY_BIT)) {}
                }

                void prev () {
                    if (slotPos == 0) {
                        return;
                    }

                    while (--slotPos > 0 && (idList->idSlots[slotPos] & EMPTY_BIT)) {}
                }
            public:
                using value_type = EntityId;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;
                IdIterator () : idList{nullptr}, slotPos{0} {}

                value_type operator* () const {
                    assert((idList->idSlots[slotPos] & EMPTY_BIT) == 0);
                    return EntityId{static_cast<unsigned int>(idList->idSlots[slotPos]), static_cast<unsigned int>(slotPos + 1)};
                }

                IdIterator& operator++ () {
                    next();
                    return *this;
                }

                IdIterator operator++ (int) {
                    auto copy = *this;
                    ++*this;

                    return copy;
                }

                IdIterator& operator-- () {
                    prev();
                    return *this;
                }

                IdIterator operator-- (int) {
                    auto copy = *this;
                    --*this;

                    return copy;
                }

                bool operator== (const IdIterator& other) const {
                    return idList == other.idList && slotPos == other.slotPos;
                }

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

            explicit EntityIdList (std::size_t capacity) : idSlots{}, freeListStart{FREE_LIST_EMPTY}, numEntities{0} {
                idSlots.reserve(capacity);
            }

            EntityId newId () {
                if (freeListStart == FREE_LIST_EMPTY) {
                    if (idSlots.size() >= MAX_NUM_IDS) {
                        component::logging::log(LEVEL_ERROR, "Too many entity ids!");
                        return EntityId{};
                    }
                    idSlots.push_back(1);
                    numEntities++;

                    assert(idSlots.size() < (std::size_t{1} << FREE_LIST_BITS));

                    return EntityId{1, static_cast<unsigned int>(idSlots.size())};
                } else {
                    auto index = freeListStart - 1;

                    assert(idSlots.size() > index);
                    assert(EMPTY_BIT & idSlots.at(index));

                    freeListStart = (idSlots.at(index) & FREE_LIST_MASK) >> GEN_BITS;
                    auto gen = static_cast<unsigned int>(idSlots.at(index) & GEN_MASK);
                    gen++;
                    if (gen == 0) {
                        gen = 1;
                    }

                    idSlots.at(index) = gen;
                    numEntities++;

                    return EntityId{gen, static_cast<unsigned int>(index + 1)};
                }
            }

            [[nodiscard]] bool check (EntityId id) const {
                if (id.id == 0 || id.id > idSlots.size()) {
                    return false;
                }

                return (idSlots[id.id - 1] & EMPTY_BIT) == 0 && idSlots[id.id - 1] == id.generation;
            }

            void removeId (EntityId id) {
                assert(id && id.id <= idSlots.size());
                assert(check(id));

                idSlots[id.id - 1] |= EMPTY_BIT | (freeListStart << GEN_BITS);
                freeListStart = id.id;
                numEntities--;
            }

            void clear () {
                for (std::size_t i = 0; i < idSlots.size(); i++) {
                    if ((idSlots[i] & EMPTY_BIT) == 0) {
                        idSlots[i] |= EMPTY_BIT | (freeListStart << GEN_BITS);
                        freeListStart = i + 1;
                    }
                }

                numEntities = 0;
            }

            [[nodiscard]] std::size_t size () const {
                return numEntities;
            }

            [[nodiscard]] std::size_t maxIndex () const {
                return idSlots.size();
            }

            [[nodiscard]] iterator begin () const {
                return cbegin();
            }
            [[nodiscard]] const_iterator cbegin () const {
                return const_iterator{this, 0};
            }

            [[nodiscard]] iterator end () const {
                return cend();
            }
            [[nodiscard]] const_iterator cend () const {
                return const_iterator{this, idSlots.size()};
            }
        };
    }

    template <typename ...Args>
    class EntityComponentView {
    private:
        std::tuple<std::remove_cvref_t<Args>&...> comps;
        EntityId id;
        explicit EntityComponentView (EntityId id, std::tuple<std::remove_cvref_t<Args>&...> comps) : id{id}, comps{comps} {}
    public:
        template <typename T>
        T& get () {
            return std::get<T&>(comps);
        }

        template <typename T>
        const T& get () const {
            return std::get<T&>(comps);
        }

        [[nodiscard]] EntityId getId () const {
            return id;
        }

        template <typename ...Args2>
        EntityComponentView<Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>,
                          "All requested types must be accessible!");
            return EntityComponentView<Args2...>{id, std::get<std::remove_cvref_t<Args2>&>(comps)...};
        }

        friend class ComponentManagerNew;
        friend class detail::ComponentView<Args...>;
    };

    template <typename ...Args>
    class ConstEntityComponentView {
    private:
        std::tuple<const std::remove_cvref_t<Args>&...> comps;
        EntityId id;
        explicit ConstEntityComponentView (EntityId id, std::tuple<const std::remove_cvref_t<Args>&...> comps) : id{id}, comps{comps} {}
    public:
        template <typename T>
        const T& get () {
            return std::get<const T&>(comps);
        }

        [[nodiscard]] EntityId getId () const {
            return id;
        }

        template <typename ...Args2>
        ConstEntityComponentView<Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>,
                          "All requested types must be accessible!");
            return ConstEntityComponentView<Args2...>{id, std::get<const std::remove_cvref_t<Args2>&>(comps)...};
        }

        friend class ComponentManagerNew;
    };

    namespace detail {
        template <typename ...Args>
        class ComponentView {
        private:
            static constexpr std::size_t NUM_ARGS = sizeof...(Args);
            class ViewIterator {
            private:
                const std::array<ComponentSet*, NUM_ARGS>* comps;
                ComponentSet* primarySet;
                std::size_t pos;

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
            public:
                using value_type = EntityComponentView<Args...>;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                ViewIterator () = default;

                value_type operator* () const {
                    return value_type{currId(), getCurrComps(currId(), std::make_index_sequence<NUM_ARGS>{})};
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
            ComponentSet* primarySet;

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
            friend class component::ComponentManagerNew;
        public:
            using iterator = ViewIterator;

            iterator begin () const {
                return iterator{&comps, primarySet, 0};
            }

            iterator end () const {
                return iterator{&comps, primarySet, primarySet->dataSize};
            }
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

                friend class ComponentView<T>;
            public:
                using value_type = EntityComponentView<T>;
                using reference = void;
                using pointer = void;
                using difference_type = std::ptrdiff_t;

                ViewIterator () = default;

                value_type operator* () const {
                    return value_type{component->ids[pos], {((T*)component->data.get())[pos]}};
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

            friend class component::ComponentManagerNew;
        public:
            using iterator = ViewIterator;

            iterator begin () {
                return iterator{component, 0};
            }

            iterator end () {
                return iterator{component, component->dataSize};
            }
        };

        static_assert(std::bidirectional_iterator<ComponentView<int>::iterator>);
    }

    class ComponentManagerNew : public util::SmartHelper<ComponentManagerNew, true> {
    private:
        class EntityView {
        private:
            EntityId id;
            ComponentManagerNew& compManager;
            EntityView (EntityId id, ComponentManagerNew& compManager) : id{id}, compManager{compManager} {}
        public:
            [[nodiscard]] EntityId getId () const {
                return id;
            }

            template <typename T>
            util::Optional<T&> getComponent () {
                return compManager.getObjectData<T>(id);
            };

            template <typename T, typename ...Args>
            void addComponent (Args&&... args) {
                compManager.addComponent<T>(id, std::forward<Args>(args)...);
            }

            template <typename T>
            void removeComponent () {
                compManager.removeComponent<T>(id);
            }

            template <typename T>
            [[nodiscard]] bool hasComponent () const {
                return compManager.hasComponent<T>(id);
            }

            void remove () {
                compManager.removeEntity(id);
            }

            friend class ComponentManagerNew;
        };

        class ConstEntityView {
        private:
            EntityId id;
            const ComponentManagerNew& compManager;
            ConstEntityView (EntityId id, const ComponentManagerNew& compManager) : id{id}, compManager{compManager} {}
        public:
            [[nodiscard]] EntityId getId () const {
                return id;
            }

            template <typename T>
            util::Optional<const T&> getComponent () const {
                return compManager.getComponent<T>(id);
            };

            template <typename T>
            [[nodiscard]] bool hasComponent () const {
                return compManager.hasComponent<T>(id);
            }

            friend class ComponentManagerNew;
        };

        class EntityViewIterator {
        private:
            detail::EntityIdList::const_iterator it;
            ComponentManagerNew* compManager;
            EntityViewIterator (ComponentManagerNew* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = ComponentManagerNew::EntityView;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            EntityViewIterator () = default;

            value_type operator* () const {
                return compManager->getEntityView(*it);
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

            friend class ComponentManagerNew;
        };

        class ConstEntityViewIterator {
        private:
            detail::EntityIdList::const_iterator it;
            const ComponentManagerNew* compManager;
            ConstEntityViewIterator (const ComponentManagerNew* compManager, detail::EntityIdList::const_iterator it) : it{it}, compManager{compManager} {}
        public:
            using value_type = ComponentManagerNew::ConstEntityView;
            using reference = void;
            using pointer = void;
            using difference_type = detail::EntityIdList::const_iterator::difference_type;

            ConstEntityViewIterator () = default;

            value_type operator* () const {
                return compManager->getEntityView(*it);
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

            friend class ComponentManagerNew;
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
        T* getEntityComp (EntityId id) {
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
        using View = ComponentManagerNew::EntityView;
        static_assert(std::bidirectional_iterator<iterator>);
        static_assert(std::bidirectional_iterator<const_iterator>);

        explicit ComponentManagerNew (std::size_t startCapacity) : idList{startCapacity}, components{}, currStartCapacity{startCapacity} {}

        template <typename T>
        void addComponentType () {
            auto typeIndex = meta::type_index<T>();
            if (components.contains(typeIndex)) {
                logging::log(LEVEL_ERROR, "Attempted to add component type of index {} that already exists!", typeIndex);
                return;
            }

            std::unique_ptr<detail::ComponentSet> component = std::make_unique<detail::ConcreteComponentSet<T>>(currStartCapacity);
            component->guaranteeEntityIndex(idList.maxIndex());

            components.emplace(typeIndex, std::move(component));
        }

        template <typename T>
        util::Optional<T&> getObjectData (EntityId id) {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<T&>{*comp} : util::Optional<T&>{};
        }

        template <typename T>
        util::Optional<const T&> getObjectData (EntityId id) const {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<T&>{*comp} : util::Optional<T&>{};
        }

        template <typename T, typename ...Args>
        void addComponent (EntityId id, Args&&... args) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to add component to invalid entity {}!", id.value());
                return;
            }
            detail::ComponentSet* comp = getOrCreateComponent<T>();

            comp->insertComp<T>(id, std::forward<Args>(args)...);
        }

        EntityView createEntity () {
            auto id = idList.newId();

            for (auto [i, comp] : components.kv()) {
                comp->guaranteeEntityIndex(id.id);
            }

            return EntityView{id, *this};
        }

        void removeEntity (EntityId id) {
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
        void removeComponent (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to remove component from invalid entity {}!", id.value());
                return;
            }
            detail::ComponentSet* comp = getComponent<T>();
            assert(comp);

            comp->deleteComp(id);
        }

        template <typename T>
        bool hasComponent (EntityId id) {
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

        std::size_t getNumObjects () const {
            return idList.size();
        }

        void clear () {
            logging::log(LEVEL_DEBUG, "Clearing entities!");
            for (auto [i, comp] : components.kv()) {
                comp->clear();
            }

            idList.clear();
        }


        // TODO: merge EntityView/ConstEntityView and EntityComponentView/ConstEntityComponentView
        EntityView getEntityView (EntityId id) {
            return EntityView{id, *this};
        }

        [[nodiscard]] ConstEntityView getEntityView (EntityId id) const {
            return ConstEntityView{id, *this};
        }

        template <typename ...Args>
        util::Optional<EntityComponentView<Args...>> getConstrainedEntityView (EntityId entityId) {
            std::tuple<std::remove_cvref_t<Args>*...> ptrs{getEntityComp<std::remove_cvref_t<Args>>(entityId)...};

            if (detail::tupleAllNonNull(ptrs)) {
                return util::Optional{EntityComponentView<Args...>{entityId, std::make_tuple(*std::get<Args>(ptrs)...)}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename ...Args>
        util::Optional<ConstEntityComponentView<Args...>> getConstrainedEntityView (EntityId entityId) const {
            std::tuple<std::remove_cvref_t<Args>*...> ptrs{getEntityComp<std::remove_cvref_t<Args>>(entityId)...};

            if (detail::tupleAllNonNull(ptrs)) {
                return util::Optional{ConstEntityComponentView<Args...>{entityId, std::make_tuple(*std::get<Args>(ptrs)...)}};
            } else {
                return util::NullOpt;
            }
        }

        template <typename ...Args>
        detail::ComponentView<std::remove_cvref_t<Args>...> getConstrainedView () {
            return detail::ComponentView<std::remove_cvref_t<Args>...>{getOrCreateComponent<std::remove_cvref_t<Args>>()...};
        }

        template <typename ...Args>
        detail::ComponentView<const Args...> getConstrainedView () const {
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

    using EntityView = ComponentManagerNew::View;
}