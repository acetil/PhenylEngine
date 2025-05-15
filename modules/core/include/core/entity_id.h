#pragma once

#include <cstddef>

namespace phenyl::core {
namespace detail {
    class ComponentSet;
    class EntityIdList;
    class BasicManager;
    class RelationshipManager;
} // namespace detail

struct EntityId {
public:
    EntityId () = default;

    EntityId (unsigned int _generation, unsigned int _id) : m_generation(_generation), m_id(_id) {}

    [[nodiscard]] std::size_t value () const noexcept {
        return (static_cast<std::size_t>(m_generation) << 32) | m_id;
    }

    explicit operator bool () const noexcept {
        return m_id != 0;
    }

    bool operator== (const EntityId& other) const noexcept {
        return m_generation == other.m_generation && m_id == other.m_id;
    }

    [[nodiscard]] unsigned int pos () const noexcept {
        return m_id - 1;
    }

    friend class Archetype;
    friend class detail::EntityIdList;
    friend class detail::RelationshipManager;

private:
    unsigned int m_generation{0};
    unsigned int m_id{0};
};

static EntityId NullId = EntityId{};
} // namespace phenyl::core
