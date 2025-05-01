#pragma once

#include "util/optional.h"

#include "core/component/archetype.h"

namespace phenyl::core {
    class Archetype;
    namespace detail {
        struct EntityEntry {
            Archetype* archetype;
            std::size_t pos;
        };
    }
    class World;
    class ChildrenView;

    class Entity {
    public:
        Entity () = default;
        Entity (EntityId id, World* entityWorld);

        [[nodiscard]] bool exists () const noexcept;
        [[nodiscard]] Entity parent () const;
        [[nodiscard]] ChildrenView children () const noexcept;
        void remove ();

        template <typename T>
        T* get () {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to get component from non-existent entity {}", id().value());
                return nullptr;
            }

            auto& e = entry();
            return e.archetype->tryGet<T>(e.pos);
        }

        template <typename T>
        const T* get () const {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to get component from non-existent entity {}", id().value());
                return nullptr;
            }

            auto& e = entry();
            return e.archetype->tryGet<T>(e.pos);
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

            if (shouldDefer()) {
                T comp{std::forward<Args>(args)...};
                deferInsert(meta::type_index<T>(), reinterpret_cast<std::byte*>(&comp));
            } else {
                e.archetype->addComponent<T>(e.pos, std::forward<Args>(args)...);
            }
        }

        template <typename T>
        void erase () {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to erase component from non-existent entity {}", id().value());
                return;
            }

            if (shouldDefer()) {
                deferErase(meta::type_index<T>());
            } else {
                auto& e = entry();
                e.archetype->removeComponent<T>(e.pos);
            }
        }

        template <typename T>
        bool has () const {
            if (!exists()) {
                PHENYL_LOGE(LOGGER, "Attempted to check component existence of non-existent entity {}", id().value());
                return false;
            }

            return entry().archetype->has<T>();
        }

        template <typename Signal>
        void raise (Signal signal) {
            raiseUntyped(meta::type_index<Signal>(), reinterpret_cast<std::byte*>(&signal));
        }

        template <typename T>
        void apply (std::function<void(T&)> applyFunc) {
            if (shouldDefer()) {
                deferApply([func = std::move(applyFunc)] (Entity entity) {
                    entity.apply(std::move(func));
                });
                return;
            }

            auto* comp = get<T>();
            if (comp) {
                applyFunc(*comp);
            }
        }

        EntityId id () const noexcept {
            return m_id;
        }

        World& world () noexcept {
            PHENYL_DASSERT(m_world);
            return *m_world;
        }

        const World& world () const noexcept {
            PHENYL_DASSERT(m_world);
            return *m_world;
        }

        void addChild (Entity child);
        Entity createChild ();

    private:
        static Logger LOGGER;

        EntityId m_id;
        World* m_world = nullptr;

        [[nodiscard]] const detail::EntityEntry& entry () const;
        void raiseUntyped (std::size_t signalType, std::byte* ptr);
        bool shouldDefer ();
        void deferInsert (std::size_t compType, std::byte* ptr);
        void deferErase (std::size_t compType);
        void deferApply (std::function<void(Entity)> applyFunc);

        friend World;
        template <typename ...Args>
        friend class ArchetypeView;
        template <typename ...Args>
        friend class Query;
    };
}