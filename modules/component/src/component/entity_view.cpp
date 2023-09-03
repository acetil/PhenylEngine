#include "component/entity_view.h"
#include "component/detail/children_view.h"

using namespace component;

ChildrenView EntityView::children () {
    return component::ChildrenView{compManager, entityId};
}
