#include "component/component.h"

using namespace component;
namespace component {
    //template class ComponentManagerWrap<entity_list>;
    template class ComponentManagerNew<PHENYL_MAX_COMPONENTS>;
    template class ComponentView<PHENYL_MAX_COMPONENTS>;
}

