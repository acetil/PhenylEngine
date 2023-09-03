#include "component/entity.h"
#include "component/detail/children_view.h"

using namespace component;

ChildrenView Entity::children () {
    return component::ChildrenView{compManager, entityId};
}
