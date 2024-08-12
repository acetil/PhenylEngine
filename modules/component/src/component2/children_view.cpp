#include "component2/children_view.h"

#include "component2/component.h"

using namespace phenyl::component;

ChildrenView2::ChildrenView2 (EntityId parentId, ComponentManager2* manager) : parentId{parentId}, manager{manager} {}

ChildrenView2::iterator ChildrenView2::begin () {
    return iterator{manager->relationships.childrenBegin(parentId), manager};
}

ChildrenView2::iterator ChildrenView2::end () {
    return iterator{manager->relationships.childrenEnd(parentId), manager};
}

ChildrenView2::const_iterator ChildrenView2::begin () const {
    return cbegin();
}

ChildrenView2::const_iterator ChildrenView2::cbegin () const {
    return const_iterator{manager->relationships.childrenBegin(parentId), manager};
}

ChildrenView2::const_iterator ChildrenView2::end () const {
    return cend();
}

ChildrenView2::const_iterator ChildrenView2::cend () const {
    return const_iterator{manager->relationships.childrenEnd(parentId), manager};
}

ChildrenView2::Iterator::Iterator () = default;
ChildrenView2::Iterator::Iterator (detail::RelationshipManager::ChildIterator it, ComponentManager2* manager) : it{it}, manager{manager} {}

ChildrenView2::Iterator::value_type ChildrenView2::Iterator::operator* () const noexcept {
    return manager->entity(*it);
}

ChildrenView2::Iterator& ChildrenView2::Iterator::operator++ () {
    ++it;
    return *this;
}

ChildrenView2::Iterator ChildrenView2::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;
    return copy;
}

bool ChildrenView2::Iterator::operator== (const Iterator& other) const noexcept {
    return it == other.it;
}




