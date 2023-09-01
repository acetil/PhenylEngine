#pragma once

#include <cstddef>

namespace component {
    namespace detail {
        class ComponentSet;
        class EntityIdList;
    }

    struct EntityId {
    private:
        unsigned int generation;
        unsigned int id;
    public:
        EntityId () = default;
        EntityId (unsigned int _generation, unsigned int _id) : generation(_generation), id(_id) {}

        [[nodiscard]] std::size_t value () const {
            return (static_cast<std::size_t>(generation) << 32) | id;
        }

        explicit operator bool () const {
            return id != 0;
        }

        friend class ComponentManager;
        friend class detail::ComponentSet;
        friend class detail::EntityIdList;
    };
}