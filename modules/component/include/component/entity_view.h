#pragma once

#include "entity_id.h"
#include "detail/basic_manager.h"

namespace component {
    class ConstEntityView {
    private:
        EntityId entityId;
        const detail::BasicComponentManager* compManager;
        ConstEntityView (EntityId id, const detail::BasicComponentManager* compManager) : entityId{id}, compManager{compManager} {}
        //ConstEntityView (EntityId id, const ComponentManager* compManager) : entityId{id}, compManager{(const detail::BasicComponentManager*)compManager} {}
        friend class ComponentManager;
        friend class detail::BasicComponentManager;
        friend class EntityView;
    public:
        [[nodiscard]] EntityId id () const {
            return entityId;
        }

        template <typename T>
        util::Optional<const T&> get () const {
            return compManager->_get<T>(entityId);
        };

        template <typename T>
        [[nodiscard]] bool has () const {
            return compManager->_has<T>(entityId);
        }

        explicit operator bool () const {
            return (bool)entityId;
        }
    };

    class EntityView {
    private:
        EntityId entityId;
        detail::BasicComponentManager* compManager;
        EntityView (EntityId id, detail::BasicComponentManager* compManager) : entityId{id}, compManager{compManager} {}
        //EntityView (EntityId id, ComponentManager* compManager) : entityId{id}, compManager{(detail::BasicComponentManager*)compManager} {}
        friend class ComponentManager;
        friend class detail::BasicComponentManager;
    public:
        EntityView () : entityId{}, compManager{nullptr} {}

        [[nodiscard]] EntityId id () const {
            return entityId;
        }

        ComponentManager& manager () {
            return *compManager->asManager();
        }

        [[nodiscard]] const ComponentManager& manager () const {
            return *compManager->asManager();
        }

        template <typename T>
        util::Optional<T&> get () {
            return compManager->_get<T>(entityId);
        };

        template <typename T, typename ...Args>
        void insert (Args&&... args) {
            compManager->_insert<T>(entityId, std::forward<Args>(args)...);
        }

        template <typename T>
        bool set (T comp) {
            return compManager->_set(entityId, std::move(comp));
        }

        template <typename T>
        void erase () {
            compManager->_erase<T>(entityId);
        }

        template <typename T>
        [[nodiscard]] bool has () const {
            return compManager->_has<T>(entityId);
        }

        template <typename ...Args, meta::callable<void, Args&...> F>
        void apply (F fn) {
            compManager->_apply<Args...>(fn, entityId);
        }

        void remove () {
            compManager->_remove(entityId);
        }

        EntityView createChild () {
            return EntityView{compManager->_create(entityId), compManager};
        }

        ChildrenView children ();

        void reparent (EntityId parent) {
            return compManager->_reparent(entityId, parent);
        }

        explicit operator bool () const {
            return (bool)entityId;
        }

        operator ConstEntityView () const {
            return ConstEntityView{entityId, compManager};
        };

        friend class ComponentManager;
    };
}