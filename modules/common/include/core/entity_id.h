#pragma once

#include <cstddef>

namespace phenyl::core {
    namespace detail {
        class ComponentSet;
        class EntityIdList;
        class BasicManager;
        class RelationshipManager;
    }

    struct EntityId {
    private:
        unsigned int generation{0};
        unsigned int id{0};
    public:
        EntityId () = default;
        EntityId (unsigned int _generation, unsigned int _id) : generation(_generation), id(_id) {}

        [[nodiscard]] std::size_t value () const noexcept {
            return (static_cast<std::size_t>(generation) << 32) | id;
        }

        explicit operator bool () const noexcept {
            return id != 0;
        }

        bool operator== (const EntityId& other) const noexcept {
            return generation == other.generation && id == other.id;
        }

        [[nodiscard]] unsigned int pos () const noexcept {
            return id - 1;
        }

        friend class Archetype;
        friend class detail::EntityIdList;
        friend class detail::RelationshipManager;
    };

    static EntityId NullId = EntityId{};
}