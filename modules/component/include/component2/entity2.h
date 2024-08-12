#pragma once

#include "util/optional.h"

#include "archetype.h"

namespace phenyl::component {
    namespace detail {
        struct EntityEntry {
            Archetype* archetype;
            std::size_t pos;
        };
    }
    class ComponentManager2;

    class Entity2 {
    private:
        static Logger LOGGER;

        EntityId entityId;
        ComponentManager2* compManager = nullptr;

        Entity2 (EntityId id, ComponentManager2* compManager);

        [[nodiscard]] const detail::EntityEntry& entry () const;
        friend ComponentManager2;
        template <typename ...Args>
        friend class ArchetypeView;
    public:
        Entity2 () = default;

        [[nodiscard]] bool exists () const noexcept;
        [[nodiscard]] Entity2 parent () const;
        void remove ();

        template <typename T>
        T* get () {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to get component from non-existent entity {}", id().value());
                return nullptr;
            }

            auto& e = entry();
            return e.archetype->get<T>(e.pos);
        }

        template <typename T>
        const T* get () const {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to get component from non-existent entity {}", id().value());
                return nullptr;
            }

            auto& e = entry();
            return e.archetype->get<T>(e.pos);
        }

        template <typename T>
        void insert (T&& comp) {
            emplace<T>(std::forward<T>(comp));
        }

        template <typename T, typename ...Args>
        void emplace (Args&&... args) {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to add component to non-existent entity {}", id().value());
                return;
            }

            auto& e = entry();
            if (e.archetype->has<T>()) {
                PHENYL_LOGE(LOGGER, "Attempted to add component to entity {} which already has it", id().value());
                return;
            }
            e.archetype->addComponent<T>(std::forward<Args>(args)...);
        }

        template <typename T>
        void erase () {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to erase component from non-existent entity {}", id().value());
                return;
            }

            auto& e = entry();
            e.archetype->removeComponent<T>(e.pos);
        }

        template <typename T>
        bool has () const {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to check component existence of non-existent entity {}", id().value());
                return false;
            }

            return entry().archetype->has<T>();
        }

        EntityId id () const noexcept {
            return entityId;
        }

        ComponentManager2& manager () noexcept {
            return *compManager;
        }

        const ComponentManager2& manager () const noexcept {
            return *compManager;
        }
    };
}