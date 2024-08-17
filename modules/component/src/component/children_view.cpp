#include "component/children_view.h"
#include "component/component.h"

using namespace phenyl::component;

ChildrenView::ChildrenView (EntityId parentId, ComponentManager* manager) : parentId{parentId}, manager{manager} {}

ChildrenView::iterator ChildrenView::begin () {
    return iterator{manager->relationships.childrenBegin(parentId), manager};
}

ChildrenView::iterator ChildrenView::end () {
    return iterator{manager->relationships.childrenEnd(parentId), manager};
}

ChildrenView::const_iterator ChildrenView::begin () const {
    return cbegin();
}

ChildrenView::const_iterator ChildrenView::cbegin () const {
    return const_iterator{manager->relationships.childrenBegin(parentId), manager};
}

ChildrenView::const_iterator ChildrenView::end () const {
    return cend();
}

ChildrenView::const_iterator ChildrenView::cend () const {
    return const_iterator{manager->relationships.childrenEnd(parentId), manager};
}

ChildrenView::Iterator::Iterator () = default;
ChildrenView::Iterator::Iterator (detail::RelationshipManager::ChildIterator it, ComponentManager* manager) : it{it}, manager{manager} {}

ChildrenView::Iterator::value_type ChildrenView::Iterator::operator* () const noexcept {
    return manager->entity(*it);
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




