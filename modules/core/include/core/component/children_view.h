#pragma once
#include "core/entity.h"
#include "core/entity_id.h"
#include "detail/relationships.h"

namespace phenyl::core {
    class World;

    class ChildrenView {
    private:
        class Iterator {
        private:
            detail::RelationshipManager::ChildIterator m_it;
            World* m_world = nullptr;

            Iterator (detail::RelationshipManager::ChildIterator it, World* world);
            friend ChildrenView;
        public:
            using value_type = Entity;
            using difference_type = std::ptrdiff_t;
            Iterator ();

            value_type operator* () const noexcept;

            Iterator& operator++ ();
            Iterator operator++ (int);

            bool operator== (const Iterator&) const noexcept;
        };

        EntityId m_parentId;
        World* m_world = nullptr;

    public:
        using const_iterator = Iterator;
        using iterator = const_iterator;

        ChildrenView (EntityId parentId, World* manager);

        iterator begin ();
        iterator end ();

        const_iterator begin () const;
        const_iterator cbegin () const;

        const_iterator end () const;
        const_iterator cend () const;
    };
}
