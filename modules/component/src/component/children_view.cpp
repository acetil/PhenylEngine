#include "component/children_view.h"
#include "component/component.h"

using namespace phenyl::component;

ChildrenView::ChildrenView (EntityId parentId, World* world) : parentId{parentId}, world{world} {}

ChildrenView::iterator ChildrenView::begin () {
    return iterator{world->relationships.childrenBegin(parentId), world};
}

ChildrenView::iterator ChildrenView::end () {
    return iterator{world->relationships.childrenEnd(parentId), world};
}

ChildrenView::const_iterator ChildrenView::begin () const {
    return cbegin();
}

ChildrenView::const_iterator ChildrenView::cbegin () const {
    return const_iterator{world->relationships.childrenBegin(parentId), world};
}

ChildrenView::const_iterator ChildrenView::end () const {
    return cend();
}

ChildrenView::const_iterator ChildrenView::cend () const {
    return const_iterator{world->relationships.childrenEnd(parentId), world};
}

ChildrenView::Iterator::Iterator () = default;
ChildrenView::Iterator::Iterator (detail::RelationshipManager::ChildIterator it, World* world) : it{it}, world{world} {}

ChildrenView::Iterator::value_type ChildrenView::Iterator::operator* () const noexcept {
    return world->entity(*it);
}

ChildrenView::Iterator& ChildrenView::Iterator::operator++ () {
    ++it;
    return *this;
}

ChildrenView::Iterator ChildrenView::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

bool ChildrenView::Iterator::operator== (const Iterator& other) const noexcept {
    return it == other.it;
}




