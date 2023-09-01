#pragma once

#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

#include "component/entity_id.h"
#include "component/forward.h"

#include "logging/logging.h"

#include "util/meta.h"

namespace component::detail {
    class ComponentSet {
    private:
        struct Metadata {
            std::size_t index;
            std::byte* data;
        };
        static constexpr std::size_t EMPTY_INDEX = -1;
        static constexpr std::size_t RESIZE_FACTOR = 2;

        std::vector<EntityId> ids;
        //std::vector<std::size_t> indexSet;
        std::vector<Metadata> metadataSet;

        std::unique_ptr<std::byte[]> data;
        std::size_t compSize;
        std::size_t dataSize;
        std::size_t dataCapacity;

        std::size_t inheritedSize{0};

        ComponentSet* parent{nullptr};
        ComponentSet* children{nullptr};

        ComponentSet* nextChild{nullptr};
        std::size_t hierachyDepth;

        template <typename T>
        inline void assertType () {
            // TODO: insert ifndef
            assertTypeIndex(meta::type_index<T>(), typeid(T).name());
        }

        std::byte* tryInsert (EntityId id);

        void guaranteeCapacity (std::size_t capacity);

        void onChildDelete (EntityId id);
        void onChildInsert (EntityId id, std::byte* ptr);
        void updateDepth (std::size_t newDepth);

        bool canInsert (EntityId id);

        friend class component::ComponentManager;
    protected:
        virtual void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const = 0;
        virtual void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) = 0;
        virtual void moveTypedComp (std::byte* dest, std::byte* src) = 0;
        virtual void deleteTypedComp (std::byte* comp) = 0;
        virtual void runDeletionCallbacks (std::byte* comp, EntityId id) = 0;
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

        bool deleteComp (EntityId id);
        void clear ();

        bool setParent (ComponentSet* parentSet);
        ComponentSet* getParent () const;
        void addChild (ComponentSet* child);
        void removeChild (ComponentSet* child);
        std::size_t getHierachyDepth () const;

        [[nodiscard]] std::size_t size () const;
    };

    template <typename T>
    class TypedComponentSet : public ComponentSet {
    private:
        std::vector<std::function<void(const T&, EntityId)>> deletionCallbacks{};
    protected:
        void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const override {
            if (typeIndex != meta::type_index<T>()) {
                logging::log(LEVEL_FATAL, "Attempted to access component of type index {} ({}) with type of index {} ()!", meta::type_index<T>(), typeid(T).name(), typeIndex, debugOtherName);
                std::exit(1);
            }
        }

        void runDeletionCallbacks (std::byte* comp, EntityId id) override {
            assert(comp);
            const T& compRef = *((T*) comp);
            for (auto& i : deletionCallbacks) {
                i(compRef, id);
            }
        }
    public:
        explicit TypedComponentSet (std::size_t startCapacity, std::size_t compSize) : ComponentSet(startCapacity, compSize) {}

        template <meta::callable<void, const T&, EntityId> F>
        void addDeletionCallback (F fn) {
            deletionCallbacks.emplace_back(std::move(fn));
        }
    };

    template <typename T>
    class ConcreteComponentSet : public TypedComponentSet<T> {
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
    public:
        explicit ConcreteComponentSet (std::size_t startCapacity) : TypedComponentSet<T>{startCapacity, sizeof(T)} {}

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
    public:
        explicit AbstractComponentSet (std::size_t startCapacity) : TypedComponentSet<T>{startCapacity, 0} {}
    };
}