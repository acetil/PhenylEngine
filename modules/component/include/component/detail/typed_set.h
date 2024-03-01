#pragma once

#include "component_set.h"
#include "loggers.h"
#include "signals/component_handler.h"

namespace phenyl::component::detail {
    template <typename T>
    class TypedComponentSet : public ComponentSet {
    protected:
        void assertTypeIndex (std::size_t typeIndex, const char* debugOtherName) const override {
            PHENYL_DASSERT_MSG(typeIndex == meta::type_index<T>(), "Attempted to access component of type index {} ({}) with type of index {} ()!", meta::type_index<T>(), typeid(T).name(), typeIndex, debugOtherName);
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
        explicit TypedComponentSet (BasicManager* manager, std::size_t startCapacity, std::size_t compSize) : ComponentSet(manager, startCapacity, compSize) {
            insertHandler = std::make_unique<TypedComponentSignalHandler<T, OnInsertUntyped>>();
            statusChangedHandler = std::make_unique<TypedComponentSignalHandler<T, OnStatusChangeUntyped>>();
            removeHandler = std::make_unique<TypedComponentSignalHandler<T, OnRemoveUntyped>>();
        }

        void addHandler (std::function<void(Entity, const OnInsert<T>&)> handler) {
            getInsertHandler().addHandler(handler);
        }

        void addHandler (std::function<void(Entity, const OnStatusChange<T>&)> handler) {
            getStatusHandler().addHandler(handler);
        }

        void addHandler (std::function<void(Entity, const OnRemove<T>&)> handler) {
            getRemoveHandler().addHandler(handler);
        }

        virtual std::size_t addPrefab (T&& comp) = 0;
    };

    template <typename T>
    class ConcreteComponentSet : public TypedComponentSet<T> {
    private:
        std::vector<std::pair<T, EntityId>> deferredInsertions{};
        util::FLVector<T> prefabs;
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

        void initPrefab (EntityId id, std::size_t prefabId) override {
            assert(prefabs.present(prefabId));
            ComponentSet::insertComp<T>(id, prefabs[prefabId]);
        }

    public:
        explicit ConcreteComponentSet (BasicManager* manager, std::size_t startCapacity) : TypedComponentSet<T>{manager, startCapacity, sizeof(T)} {

        }

        ~ConcreteComponentSet () override {
            ComponentSet::clear();
        }

        std::size_t addPrefab (T&& comp) override {
            return prefabs.push(std::move(comp));
        }

        void deletePrefab (std::size_t prefabId) override {
            assert(prefabs.present(prefabId));
            prefabs.remove(prefabId);
        }
    };

    template <typename T>
    class AbstractComponentSet : public TypedComponentSet<T> {
    protected:
        void moveTypedComp(std::byte *dest, std::byte *src) override {
            PHENYL_ABORT("Attempted to move comp of abstract component set!");
        }

        void deleteTypedComp(std::byte *comp) override {
            PHENYL_ABORT("Attempted to delete comp of abstract component set!");
        }

        void moveAllComps (std::byte* dest, std::byte* src, std::size_t len) override {
            PHENYL_ABORT("Attempted to move all comps of abstract component set!");
        }

        void swapTypedComp (std::byte* ptr1, std::byte* ptr2) override {
            PHENYL_ABORT("Attempted to swap comps of abstract component set!");
        }

        void deferInsertion (std::byte* comp, EntityId id) override {
            PHENYL_ABORT("Attempted to defer insertion of abstract comp!");
        }

        void popDeferredInsertions () override {

        }

        void initPrefab (EntityId id, std::size_t prefabId) override {
            PHENYL_ABORT("Attempted to init prefab of abstract comp!");
        }

        std::size_t addPrefab (T&& comp) override {
            PHENYL_ABORT("Attempted to add prefab of abstract comp!");
        }

        void deletePrefab (std::size_t prefabId) override {
            PHENYL_ABORT("Attempted to remove prefab of abstract comp!");
        }
    public:
        explicit AbstractComponentSet (BasicManager* manager, std::size_t startCapacity) : TypedComponentSet<T>{manager, startCapacity, 0} {}
    };
}