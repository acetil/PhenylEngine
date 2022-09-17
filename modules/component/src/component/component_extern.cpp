#include "component/component.h"
#include "component/component_serialiser.h"

using namespace component;
namespace component {
    //template class ComponentManagerWrap<entity_list>;
    template class ComponentManager<PHENYL_MAX_COMPONENTS>;
    template class ComponentView<PHENYL_MAX_COMPONENTS>;

    template class ObjectSerialiser<PHENYL_MAX_COMPONENTS>;
    template class ObjectComponentFactory<PHENYL_MAX_COMPONENTS>;
}

