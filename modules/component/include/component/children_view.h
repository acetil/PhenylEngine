#pragma once
#include "entity.h"
#include "entity_id.h"
#include "detail/relationships.h"

namespace phenyl::component {
    class ComponentManager;

    class ChildrenView {
    private:
        class Iterator {
        private:
            detail::RelationshipManager::ChildIterator it;
            ComponentManager* manager = nullptr;

            Iterator (detail::RelationshipManager::ChildIterator it, ComponentManager* manager);
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

        EntityId parentId;
        ComponentManager* manager = nullptr;

    public:
        using const_iterator = Iterator;
        using iterator = const_iterator;

        ChildrenView (EntityId parentId, ComponentManager* manager);

        iterator begin ();
        iterator end ();

        const_iterator begin () const;
        const_iterator cbegin () const;

        const_iterator end () const;
        const_iterator cend () const;
    };
}
