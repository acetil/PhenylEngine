#pragma once

#include "component/forward.h"
#include "component/entity_id.h"

#include "component/detail/component_utils.h"
#include "component/detail/component_set.h"
#include "component/detail/entity_id_list.h"
#include "component/detail/signals/handler_list.h"
#include "component/detail/relationships.h"

#include "util/map.h"
#include "util/meta.h"
#include "util/optional.h"

namespace component::detail {
    class BasicManager {
    protected:
        EntityIdList idList;
        util::Map<std::size_t, std::unique_ptr<ComponentSet>> components;
        util::Map<std::size_t, std::unique_ptr<detail::SignalHandlerListBase>> signalHandlers;
        std::size_t deferCount{0};
        std::size_t signalDeferCount{0};
        std::vector<EntityId> deferredDeletions{};
        std::vector<std::pair<std::function<void(BasicManager*, EntityId)>, EntityId>> deferredApplys;
        RelationshipManager relationships;

        explicit BasicManager (std::size_t startCapacity) : idList{startCapacity}, components{}, relationships{startCapacity} {}

        void removeInt (EntityId id, bool updateParent) {
            if (updateParent) {
                signalRemoveChild(relationships.parent(id), id);
            }

            auto curr = relationships.entityChildren(id);
            while (curr) {
                auto next = relationships.next(curr);
                removeInt(curr, false);
                curr = next;
            }

            relationships.remove(id, updateParent);

            for (auto [i, comp] : components.kv()) {
                comp->deleteComp(id);
            }

            idList.removeId(id);
        }

        template <typename ...Args, meta::callable<void, Args&...> F>
        inline void applyNow (F& fn, EntityId id) {
            std::tuple<Args*...> compTup{getEntityComp<Args>(id)...};
            if (tupleAllNonNull<Args...>(compTup)) {
                fn((*std::get<Args*>(compTup))...);
            }
        }

        void onSignalDeferBegin () {
            for (auto [k, v] : signalHandlers.kv()) {
                v->defer();
            }
        }

        void onDeferBegin () {
            if (!signalDeferCount) {
                onSignalDeferBegin();
            }

            for (auto [k, v] : components.kv()) {
                v->defer();
            }
        }

        void onSignalDeferEnd () {
            for (auto [k, v] : signalHandlers.kv()) {
                v->deferEnd(this);
            }
        }

        void onDeferEnd () {
            for (auto [k, v] : components.kv()) {
                v->deferEnd();
            }

            if (!signalDeferCount) {
                onSignalDeferEnd();
            }

            for (auto& [f, id] : deferredApplys) {
                f(this, id);
            }

            for (auto i : deferredDeletions) {
                _remove(i);
            }

            deferredApplys.clear();
            deferredDeletions.clear();
        }

        template <typename Signal>
        SignalHandlerList<Signal>* getHandlerList () {
            auto typeIndex = meta::type_index<Signal>();
            if (signalHandlers.contains(typeIndex)) {
                return (SignalHandlerList<Signal>*)signalHandlers[typeIndex].get();
            } else {
                return nullptr;
            }
        }

        void signalAddChild (EntityId id, EntityId child);
        void signalRemoveChild (EntityId id, EntityId oldChild);
    public:
        template <typename T>
        ComponentSet* getComponent () const {
            auto typeIndex = meta::type_index<T>();
            if (!components.contains(typeIndex)) {
                logging::log(LEVEL_ERROR, "Failed to get component for index {}!", typeIndex);
                return nullptr;
            }

            return components.at(typeIndex).get();
        }

        template <typename T>
        T* getEntityComp (EntityId id) const {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to get component from invalid entity {}!", id.value());
                return nullptr;
            }
            ComponentSet* component = getComponent<T>();

            return component ? component->getComponent<T>(id) : nullptr;
        }

        EntityId _create (EntityId parent) {
            auto id = idList.newId();

            for (auto [i, comp] : components.kv()) {
                comp->guaranteeEntityIndex(id.id);
            }

            relationships.add(id, parent);
            signalAddChild(parent, id);

            return id;
        }

        [[nodiscard]] bool _exists (EntityId id) const {
            return idList.check(id);
        }

        void _remove (EntityId id) {
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

        template <typename ...Args, meta::callable<void, Args&...> F>
        void _apply (F fn, EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to apply on invalid entity {}!", id.value());
                return;
            }

            if (!deferCount) {
                applyNow<Args...>(fn, id);
                return;
            }

            auto applyFn = [fn=std::move(fn)] (BasicManager* manager, EntityId id) {
                manager->applyNow<Args...>(fn, id);
            };
            deferredApplys.emplace_back(std::move(applyFn), id);
        }

        ChildrenView _children (EntityId id);

        void _reparent (EntityId id, EntityId parent) {
            signalRemoveChild(relationships.parent(id), id);
            relationships.removeFromParent(id);
            relationships.setParent(id, parent);
            signalAddChild(parent, id);
        }

        [[nodiscard]] EntityId _parent (EntityId id) const {
            return relationships.parent(id);
        }

        void _defer () {
            if (deferCount++ == 0) {
                onDeferBegin();
            }
        }

        void _deferEnd () {
            if (--deferCount == 0) {
                onDeferEnd();
            }
        }

        void _deferSignals () {
            if (signalDeferCount++ == 0 && !deferCount) {
                onSignalDeferBegin();
            }
        }

        void _deferSignalsEnd () {
            if (--signalDeferCount == 0 && !deferCount) {
                onSignalDeferEnd();
            }
        }

        template <typename Signal>
        void _signal (EntityId id, Signal signal) requires (!ComponentSignal<Signal>) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to signal invalid entity {}!", id.value());
                return;
            }

            SignalHandlerList<Signal>* handlerList = getHandlerList<Signal>();

            if (handlerList) {
                handlerList->handle(id, this, std::move(signal));
            }
        }


        ComponentManager* asManager () {
            return (ComponentManager*)this;
        }

        [[nodiscard]] const ComponentManager* asManager () const {
            return (ComponentManager*)this;
        }

        Entity _entity (EntityId id);
        [[nodiscard]] ConstEntity _entity (EntityId id) const;
        [[nodiscard]] ConstEntity _constEntity (EntityId id) const;

        RelationshipManager& getRelationshipManager () {
            return relationships;
        }
    };
}