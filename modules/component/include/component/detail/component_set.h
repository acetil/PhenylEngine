#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "component/entity_id.h"
#include "component/forward.h"
#include "component_signal_handler.h"

#include "logging/logging.h"

#include "util/meta.h"

namespace component::detail {
    class ComponentSet {
    private:
        struct Metadata {
            static constexpr std::uint32_t EMPTY_INDEX = -1;
            static constexpr std::uint32_t DEPENDENCY_BITS = 8;
            static constexpr std::uint32_t DEPENDENCY_MASK = (std::uint32_t{1} << DEPENDENCY_BITS) - 1;
            static constexpr std::uint32_t MAX_DEPENDENCY_NUM = (std::uint32_t{1} << DEPENDENCY_BITS) - 1;
            static constexpr std::uint32_t CHILD_BIT = std::uint32_t{1} << DEPENDENCY_BITS;
            static constexpr std::uint32_t METADATA_MASK = 0xFFFFFFFF;

            std::uint32_t index;
            std::uint32_t metadata;
            std::byte* data;

            [[nodiscard]] inline constexpr bool empty () const {
                return !present() && !(metadata & CHILD_BIT);
            }

            [[nodiscard]] inline constexpr bool present () const {
                return index != EMPTY_INDEX;
            }

            [[nodiscard]] inline constexpr bool active () const {
                return !(metadata & DEPENDENCY_MASK);
            }

            inline constexpr void clear () {
                index = EMPTY_INDEX;
                data = nullptr;
                metadata &= (METADATA_MASK ^ CHILD_BIT);
            }

            static constexpr Metadata Empty (std::uint32_t dependencySize) {
                assert(dependencySize < (1 << DEPENDENCY_BITS));
                return {.index = EMPTY_INDEX, .metadata = dependencySize, .data = nullptr};
            }

            /*static constexpr Metadata Filled (std::uint32_t index, std::byte* data) {
                assert(data);
                return {.index = index, .data = data};
            }*/
            inline constexpr void fill (std::uint32_t index, std::byte* data) {
                //assert(empty());
                this->index = index;
                this->data = data;
            }

            inline constexpr void fillChild (std::byte* data) {
                //assert(index == EMPTY_INDEX);
                this->data = data;
                this->metadata |= CHILD_BIT;
            }

            inline constexpr void move (std::uint32_t newIndex, std::byte* newData) {
                assert(!(metadata & CHILD_BIT));
                index = newIndex;
                data = newData;
            }

            inline constexpr void incrementDepenencies () {
                assert((metadata & DEPENDENCY_MASK) < MAX_DEPENDENCY_NUM);
                auto nonDependencies = metadata & (METADATA_MASK ^ DEPENDENCY_MASK);

                metadata = nonDependencies | ((metadata & METADATA_MASK) + 1);
            }

            inline constexpr void decrementDependencies () {
                assert((metadata & DEPENDENCY_MASK) > 0);
                auto nonDependencies = metadata & (METADATA_MASK ^ DEPENDENCY_MASK);

                metadata = nonDependencies | ((metadata & METADATA_MASK) - 1);
            }
        };
        static constexpr std::uint32_t EMPTY_INDEX = -1;
        static constexpr std::size_t RESIZE_FACTOR = 2;

        ComponentManager* manager;
        std::vector<EntityId> ids;
        //std::vector<std::size_t> indexSet;
        std::vector<Metadata> metadataSet;

        std::unique_ptr<std::byte[]> data;
        std::size_t compSize;
        std::size_t dataSize;
        std::size_t allSize;
        std::size_t dataCapacity;

        std::size_t inheritedSize{0};

        ComponentSet* parent{nullptr};
        ComponentSet* children{nullptr};

        ComponentSet* nextChild{nullptr};
        std::size_t hierachyDepth;

        std::vector<ComponentSet*> dependents;
        std::uint32_t dependencySize{0};

        bool deferring = false;
        std::vector<EntityId> deferredDeletions;

        template <typename T>
        inline void assertType () {
            // TODO: insert ifndef
            assertTypeIndex(meta::type_index<T>(), typeid(T).name());
        }

        std::byte* tryInsert (EntityId id);

        void guaranteeCapacity (std::size_t capacity);

        void onChildDelete (EntityId id);
        void onChildInsert (EntityId id, std::byte* ptr);
        void onChildUpdate (EntityId id, std::byte* ptr);
        void onChildRelocate (EntityId id, std::byte* ptr);

        void updateDepth (std::size_t newDepth);

        bool canInsert (EntityId id);

        void onInsert (EntityId id, std::byte* ptr, std::byte* childPtr);
        void onRemove (EntityId id);

        void activate (EntityId id);
        void deactivate (EntityId id);

        friend class component::ComponentManager;
    protected:
        std::unique_ptr<ComponentSignalHandler<OnInsertUntyped>> insertHandler{};
        std::unique_ptr<ComponentSignalHandler<OnStatusChangeUntyped>> statusChangedHandler{};
        std::unique_ptr<ComponentSignalHandler<OnRemoveUntyped>> removeHandler{};

        virtual void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const = 0;
        virtual void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) = 0;
        virtual void moveTypedComp (std::byte* dest, std::byte* src) = 0;
        virtual void deleteTypedComp (std::byte* comp) = 0;
        virtual void swapTypedComp (std::byte* ptr1, std::byte* ptr2) = 0;
        virtual void deferInsertion (std::byte* comp, EntityId id) = 0;
        virtual void popDeferredInsertions () = 0;
    public:
        ComponentSet (ComponentManager* manager, std::size_t startCapacity, std::size_t compSize);
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
        bool insertComp (EntityId id, Args&&... args) {
            assertType<T>();
            if (deferring) {
                T comp{std::forward<Args>(args)...};
                deferInsertion((std::byte*)(&comp), id);
                return true;
            }

            auto* comp = tryInsert(id);

            if (comp) {
                new ((T*)comp) T(std::forward<Args>(args)...);

                IterInfo info{manager, id};
                insertHandler->handle(info, OnInsertUntyped{comp});
                return true;
            } else {
                return false;
            }
        }

        template <typename T>
        bool setComp (EntityId id, T comp) {
            assertType<T>();
            assert(id);

            if (metadataSet[id.id - 1].empty()) {
                return insertComp<T>(id, std::move(comp));
            } else if (metadataSet[id.id - 1].present()) {
                *getComponent<T>(id) = std::move(comp);
                return true;
            } else {
                return false;
            }
        }

        bool hasComp (EntityId id) const;

        bool deleteComp (EntityId id);
        void clear ();

        bool setParent (ComponentSet* parentSet);
        ComponentSet* getParent () const;
        void addChild (ComponentSet* child);
        void removeChild (ComponentSet* child);
        std::size_t getHierachyDepth () const;

        void addDependency ();
        void addDependent (ComponentSet* dependent);
        void onDependencyInsert (EntityId id);
        void onDependencyRemove (EntityId id);

        void defer ();
        void deferEnd ();

        [[nodiscard]] std::size_t size () const;
    };

    template <typename T>
    class TypedComponentSet : public ComponentSet {
    protected:
        void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const override {
            if (typeIndex != meta::type_index<T>()) {
                logging::log(LEVEL_FATAL, "Attempted to access component of type index {} ({}) with type of index {} ()!", meta::type_index<T>(), typeid(T).name(), typeIndex, debugOtherName);
                std::exit(1);
            }
        }

        TypedComponentSignalHandler<T, OnInsertUntyped>& getInsertHandler () {
            return (TypedComponentSignalHandler<T, OnInsertUntyped>&)*insertHandler;
        }

        TypedComponentSignalHandler<T, OnStatusChangeUntyped>& getStatusHandler () {
            return (TypedComponentSignalHandler<T, OnStatusChangeUntyped>&)*statusChangedHandler;
        }

        TypedComponentSignalHandler<T, OnRemoveUntyped>& getRemoveHandler () {
            return (TypedComponentSignalHandler<T, OnRemoveUntyped>&)*removeHandler;
        }
    public:
        explicit TypedComponentSet (ComponentManager* manager, std::size_t startCapacity, std::size_t compSize) : ComponentSet(manager, startCapacity, compSize) {
            insertHandler = std::make_unique<TypedComponentSignalHandler<T, OnInsertUntyped>>();
            statusChangedHandler = std::make_unique<TypedComponentSignalHandler<T, OnStatusChangeUntyped>>();
            removeHandler = std::make_unique<TypedComponentSignalHandler<T, OnRemoveUntyped>>();
        }

        void addHandler (std::function<void(IterInfo&, const OnInsert<T>&)> handler) {
            getInsertHandler().addHandler(handler);
        }

        void addHandler (std::function<void(IterInfo&, const OnStatusChange<T>&)> handler) {
            getStatusHandler().addHandler(handler);
        }

        void addHandler (std::function<void(IterInfo&, const OnRemove<T>&)> handler) {
            getRemoveHandler().addHandler(handler);
        }
    };

    template <typename T>
    class ConcreteComponentSet : public TypedComponentSet<T> {
    private:
        std::vector<std::pair<T, EntityId>> deferredInsertions{};
    protected:
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

        void swapTypedComp (std::byte* ptr1, std::byte* ptr2) override {
            using std::swap;
            swap(*((T*)ptr1), *((T*)ptr2));
        }

        void deferInsertion (std::byte* comp, EntityId id) override {
            T* compPtr = (T*)comp;

            deferredInsertions.emplace_back(std::move(*compPtr), id);
        }

        void popDeferredInsertions () override {
            for (auto& i : deferredInsertions) {
                ComponentSet::insertComp<T>(i.second, std::move(i.first));
            }

            deferredInsertions.clear();
        }

    public:
        explicit ConcreteComponentSet (ComponentManager* manager, std::size_t startCapacity) : TypedComponentSet<T>{manager, startCapacity, sizeof(T)} {

        }

        ~ConcreteComponentSet () override {
            ComponentSet::clear();
        }
    };

    template <typename T>
    class AbstractComponentSet : public TypedComponentSet<T> {
    protected:
        void moveTypedComp(std::byte *dest, std::byte *src) override {
            logging::log(LEVEL_FATAL, "Attempted to move comp of abstract component set!");
            assert(false);
        }

        void deleteTypedComp(std::byte *comp) override {
            logging::log(LEVEL_FATAL, "Attempted to delete comp of abstract component set!");
            assert(false);
        }

        void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) override {
            logging::log(LEVEL_FATAL, "Attempted to move all comps of abstract component set!");
            assert(false);
        }

        void swapTypedComp (std::byte* ptr1, std::byte* ptr2) override {
            logging::log(LEVEL_FATAL, "Attempted to swap comps of abstract component set!");
            assert(false);
        }

        void deferInsertion (std::byte* comp, EntityId id) override {
            logging::log(LEVEL_FATAL, "Attempted to defer insertion of abstract comp!");
            assert(false);
        }

        void popDeferredInsertions () override {

        }
    public:
        explicit AbstractComponentSet (ComponentManager* manager, std::size_t startCapacity) : TypedComponentSet<T>{manager, startCapacity, 0} {}
    };
}