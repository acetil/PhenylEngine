#include "core/component/children_view.h"

#include "core/world.h"

using namespace phenyl::core;

ChildrenView::ChildrenView (EntityId parentId, World* world) : m_parentId{parentId}, m_world{world} {}

ChildrenView::iterator ChildrenView::begin () {
    return iterator{m_world->m_relationships.childrenBegin(m_parentId), m_world};
}

ChildrenView::iterator ChildrenView::end () {
    return iterator{m_world->m_relationships.childrenEnd(m_parentId), m_world};
}

ChildrenView::const_iterator ChildrenView::begin () const {
    return cbegin();
}

ChildrenView::const_iterator ChildrenView::cbegin () const {
    return const_iterator{m_world->m_relationships.childrenBegin(m_parentId), m_world};
}

ChildrenView::const_iterator ChildrenView::end () const {
    return cend();
}

ChildrenView::const_iterator ChildrenView::cend () const {
    return const_iterator{m_world->m_relationships.childrenEnd(m_parentId), m_world};
}

ChildrenView::Iterator::Iterator () = default;

ChildrenView::Iterator::Iterator (detail::RelationshipManager::ChildIterator it, World* world) :
    m_it{it},
    m_world{world} {}

ChildrenView::Iterator::value_type ChildrenView::Iterator::operator* () const noexcept {
    return m_world->entity(*m_it);
}

ChildrenView::Iterator& ChildrenView::Iterator::operator++ () {
    ++m_it;
    return *this;
}

ChildrenView::Iterator ChildrenView::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

bool ChildrenView::Iterator::operator== (const Iterator& other) const noexcept {
    return m_it == other.m_it;
}
