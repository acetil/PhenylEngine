#pragma once
#include "entity2.h"
#include "component/entity_id.h"
#include "component/detail/relationships.h"

namespace phenyl::component {
    class ComponentManager2;

    class ChildrenView2 {
    private:
        class Iterator {
        private:
            detail::RelationshipManager::ChildIterator it;
            ComponentManager2* manager = nullptr;

            Iterator (detail::RelationshipManager::ChildIterator it, ComponentManager2* manager);
            friend ChildrenView2;
        public:
            using value_type = Entity2;
            using difference_type = std::ptrdiff_t;
            Iterator ();

            value_type operator* () const noexcept;

            Iterator& operator++ ();
            Iterator operator++ (int);

            bool operator== (const Iterator&) const noexcept;
        };

        EntityId parentId;
        ComponentManager2* manager = nullptr;

    public:
        using const_iterator = Iterator;
        using iterator = const_iterator;

        ChildrenView2 (EntityId parentId, ComponentManager2* manager);

        iterator begin ();
        iterator end ();

        const_iterator begin () const;
        const_iterator cbegin () const;

        const_iterator end () const;
        const_iterator cend () const;
    };
}
