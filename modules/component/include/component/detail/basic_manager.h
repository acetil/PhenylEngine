#pragma once

#include "component/forward.h"
#include "component/entity_id.h"

#include "component_utils.h"
#include "component_set.h"
#include "entity_id_list.h"
#include "signal_handler.h"
#include "relationships.h"

#include "util/map.h"
#include "util/meta.h"
#include "util/optional.h"

namespace component::detail {
    class BasicComponentManager {
    protected:
        EntityIdList idList;
        util::Map<std::size_t, std::unique_ptr<ComponentSet>> components;
        std::size_t deferCount{0};
        std::vector<EntityId> deferredDeletions{};
        std::vector<std::pair<std::function<void(BasicComponentManager*, EntityId)>, EntityId>> deferredApplys;
        RelationshipManager relationships;

        explicit BasicComponentManager (std::size_t startCapacity) : idList{startCapacity}, components{}, relationships{startCapacity} {}

        void removeInt (EntityId id, bool updateParent) {
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

        /*template <typename T>
        util::Optional<T&> _get (EntityId id) {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<T&>{*comp} : util::Optional<T&>{};
        }

        template <typename T>
        util::Optional<const T&> _get (EntityId id) const {
            auto* comp = getEntityComp<T>(id);

            return comp ? util::Optional<const T&>{*comp} : util::Optional<const T&>{};
        }

        template <typename T, typename ...Args>
        void _insert (EntityId id, Args&&... args) requires std::constructible_from<T, Args...> {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to insert component to invalid entity {}!", id.value());
                return;
            }

            ComponentSet* comp = getComponent<T>();
            if (!comp) {
                return;
            }

            comp->insertComp<T>(id, std::forward<Args>(args)...);
        }

        template <typename T>
        bool _set (EntityId id, T comp) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to set component of invalid entity {}!", id.value());
                return false;
            }
            ComponentSet* compSet = getComponent<T>();
            if (!compSet) {
                return false;
            }

            return compSet->setComp(id, std::move(comp));
        }*/

        EntityId _create (EntityId parent) {
            auto id = idList.newId();

            for (auto [i, comp] : components.kv()) {
                comp->guaranteeEntityIndex(id.id);
            }

            relationships.add(id, parent);

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

        /*template <typename T>
        void _erase (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to remove component from invalid entity {}!", id.value());
                return;
            }
            ComponentSet* comp = getComponent<T>();
            if (!comp) {
                return;
            }

            comp->deleteComp(id);
        }

        template <typename T>
        bool _has (EntityId id) {
            if (!idList.check(id)) {
                logging::log(LEVEL_ERROR, "Attempted to check component status for invalid entity {}!", id.value());
                return false;
            }
            ComponentSet* comp = getComponent<T>();
            if (!comp) {
                logging::log(LEVEL_ERROR, "Attempted to check component status of component with index {} that doesn't exist!", meta::type_index<T>());
                return false;
            }

            return comp->hasComp(id);
        }*/

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

            auto applyFn = [fn=std::move(fn)] (BasicComponentManager* manager, EntityId id) {
                manager->applyNow<Args...>(fn, id);
            };
            deferredApplys.emplace_back(std::move(applyFn), id);
        }

        ChildrenView _children (EntityId id);

        void _reparent (EntityId id, EntityId parent) {
            relationships.removeFromParent(id);
            relationships.setParent(id, parent);
        }

        [[nodiscard]] EntityId _parent (EntityId id) const {
            return relationships.parent(id);
        }

        ComponentManager* asManager () {
            return (ComponentManager*)this;
        }

        [[nodiscard]] const ComponentManager* asManager () const {
            return (ComponentManager*)this;
        }

        Entity _view (EntityId id);
        [[nodiscard]] ConstEntity _view (EntityId id) const;

        RelationshipManager& getRelationshipManager () {
            return relationships;
        }
    };
}