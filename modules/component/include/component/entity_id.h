#pragma once

#include <cstddef>

namespace component {
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

        [[nodiscard]] std::size_t value () const {
            return (static_cast<std::size_t>(generation) << 32) | id;
        }

        explicit operator bool () const {
            return id != 0;
        }

        bool operator== (const EntityId& other) const {
            return generation == other.generation && id == other.id;
        }

        friend class ComponentManager;
        friend class detail::ComponentSet;
        friend class detail::EntityIdList;
        friend class detail::BasicManager;
        friend class detail::RelationshipManager;
    };
}