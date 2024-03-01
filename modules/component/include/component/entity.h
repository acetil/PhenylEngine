#pragma once

#include "entity_id.h"
#include "component/detail/managers/basic_manager.h"

namespace phenyl::component {
    class Entity;
    class ConstEntity;
    inline bool operator== (const Entity& entity1, const Entity& entity2);
    inline bool operator== (const Entity& entity1, const ConstEntity& entity2);
    inline bool operator== (const ConstEntity& entity1, const ConstEntity& entity2);

    class ConstEntity {
    private:
        EntityId entityId;
        const detail::BasicManager* compManager;
        ConstEntity (EntityId id, const detail::BasicManager* compManager) : entityId{id}, compManager{compManager} {}
        friend class detail::BasicManager;
        friend class Entity;
    public:
        [[nodiscard]] EntityId id () const {
            return entityId;
        }

        [[nodiscard]] const ComponentManager& manager () const {
            return *compManager->asManager();
        }

        [[nodiscard]] ConstEntity parent () const {
            return compManager->_entity(compManager->_parent(entityId));
        }

        template <typename T>
        util::Optional<const T&> get () const {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to get component of invalid entity {}!", entityId.value());
                return util::NullOpt;
            }

            detail::ComponentSet* compSet;
            if (!((compSet = compManager->getComponent<T>()))) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to get component for entity {}!", entityId.value());
                return util::NullOpt;
            }

            T* comp = compSet->getComponent<T>(entityId);
            return comp ? util::Optional<const T&>{*comp} : util::Optional<const T&>{};
        };

        template <typename T>
        [[nodiscard]] bool has () const {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to check component for invalid entity {}!", entityId.value());
                return false;
            }

            detail::ComponentSet* compSet = compManager->getComponent<T>();
            return compSet && compSet->hasComp(entityId);
        }

        [[nodiscard]] bool exists () const {
            return compManager->_exists(entityId);
        }

        explicit operator bool () const {
            return (bool)entityId;
        }
    };

    class Entity {
    private:
        EntityId entityId;
        detail::BasicManager* compManager;
        Entity (EntityId id, detail::BasicManager* compManager) : entityId{id}, compManager{compManager} {}
        friend class detail::BasicManager;
    public:
        Entity () : entityId{}, compManager{nullptr} {}

        [[nodiscard]] EntityId id () const {
            return entityId;
        }

        ComponentManager& manager () {
            return *compManager->asManager();
        }

        [[nodiscard]] const ComponentManager& manager () const {
            return *compManager->asManager();
        }

        Entity parent () {
            return compManager->_entity(compManager->_parent(entityId));
        }

        [[nodiscard]] ConstEntity parent () const {
            return compManager->_entity(compManager->_parent(entityId));
        }

        template <typename T>
        util::Optional<T&> get () {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to get component of invalid entity {}!", entityId.value());
                return util::NullOpt;
            }

            detail::ComponentSet* compSet;
            if (!((compSet = compManager->getComponent<T>()))) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to get component for entity {}!", entityId.value());
                return util::NullOpt;
            }

            T* comp = compSet->getComponent<T>(entityId);
            return comp ? util::Optional<T&>{*comp} : util::Optional<T&>{};
        }

        template <typename T>
        util::Optional<const T&> get () const {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to get component of invalid entity {}!", entityId.value());
                return util::NullOpt;
            }

            detail::ComponentSet* compSet;
            if (!((compSet = compManager->getComponent<T>()))) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to get component for entity {}!", entityId.value());
                return util::NullOpt;
            }

            T* comp = compSet->getComponent<T>(entityId);
            return comp ? util::Optional<const T&>{*comp} : util::Optional<const T&>{};
        };

        template <typename T>
        void insert (T comp) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to insert component in invalid entity {}!", entityId.value());
                return;
            }

            detail::ComponentSet* compSet;
            if ((compSet = compManager->getComponent<T>())) {
                compSet->insertComp<T>(entityId, std::move(comp));
            } else {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to insert component in entity {}!", entityId.value());
            }
        }

        template <typename T, typename ...Args>
        void emplace (Args&&... args) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to emplace component in invalid entity {}!", entityId.value());
                return;
            }

            detail::ComponentSet* compSet;
            if ((compSet = compManager->getComponent<T>())) {
                compSet->insertComp<T>(entityId, std::forward<Args>(args)...);
            } else {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to emplace component in entity {}!", entityId.value());
            }
        }

        template <typename T>
        bool set (T comp) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to set component in invalid entity {}!", entityId.value());
                return false;
            }

            detail::ComponentSet* compSet;
            if ((compSet = compManager->getComponent<T>())) {
                return compSet->setComp(entityId, std::move(comp));
            } else {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to set component in entity {}!", entityId.value());
                return false;
            }
        }

        template <typename T>
        void erase () {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to erase component in invalid entity {}!", entityId.value());
                return;
            }

            detail::ComponentSet* compSet;
            if ((compSet = compManager->getComponent<T>())) {
                compSet->deleteComp(entityId);
            } else {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Failed to erase component in entity {}!", entityId.value());
            }
        }

        template <typename T>
        [[nodiscard]] bool has () const {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to check component for invalid entity {}!", entityId.value());
                return false;
            }

            detail::ComponentSet* compSet = compManager->getComponent<T>();
            return compSet && compSet->hasComp(entityId);
        }

        template <typename ...Args, meta::callable<void, Args&...> F>
        Entity& apply (F fn) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to apply on invalid entity {}!", entityId.value());
                return *this;
            }

            compManager->_apply<Args...>(fn, entityId);
            return *this;
        }

        template <typename Signal>
        void signal (Signal signal) requires (!ComponentSignal<Signal>) {
            compManager->_signal(entityId, std::move(signal));
        }

        template <typename Signal, typename ...Args>
        void signal (Args&&... args) requires (!ComponentSignal<Signal>) {
            compManager->_signal(entityId, Signal{std::forward<Args>(args)...});
        }

        void remove () {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to remove invalid entity {}!", entityId.value());
                return;
            }

            compManager->_remove(entityId);
        }

        Entity createChild () {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to create child for invalid entity {}!", entityId.value());
                return {};
            }

            return Entity{compManager->_create(entityId), compManager};
        }

        void addChild (Entity child) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to add child to invalid entity {}!", entityId.value());
                return;
            }

            compManager->_reparent(child.id(), entityId);
        }

        void detach () {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to detach invalid entity {}!", entityId.value());
                return;
            }

            if (!compManager->_parent(entityId)) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to detach root entity {}!", entityId.value());
                return;
            }

            compManager->_reparent(entityId, EntityId{});
        }

        void detachChild (Entity child) {
            if (!exists()) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to detach {} from invalid {}!", child.id().value(), entityId.value());
                return;
            }

            if (child.parent() != *this) {
                PHENYL_LOGE(detail::ENTITY_LOGGER, "Attempted to detach {} from non-parent {}! (parent: {})", child.id().value(), entityId.value(), child.parent().id().value());
                return;
            }

            compManager->_reparent(child.id(), EntityId{});
        }

        ChildrenView children ();

        [[nodiscard]] bool exists () const {
            return compManager->_exists(entityId);
        }

        explicit operator bool () const {
            return (bool)entityId;
        }

        operator ConstEntity () const {
            return ConstEntity{entityId, compManager};
        };

        friend class ComponentManager;
    };

    static Entity Null = Entity{};

    inline bool operator== (const Entity& entity1, const Entity& entity2) {
        return &entity1.manager() == &entity2.manager() && entity1.id() == entity2.id();
    }

    inline bool operator== (const Entity& entity1, const ConstEntity& entity2) {
        return &entity1.manager() == &entity2.manager() && entity1.id() == entity2.id();
    }

    inline bool operator== (const ConstEntity& entity1, const ConstEntity& entity2) {
        return &entity1.manager() == &entity2.manager() && entity1.id() == entity2.id();
    }
}