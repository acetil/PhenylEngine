#include "component/components/2D/position.h"
#include "util/data.h"

using namespace component;

util::DataValue component::phenyl_to_data (const component::Position2D& comp) {
    return (util::DataValue)comp.get();
}

bool component::phenyl_from_data (const util::DataValue& dataVal, component::Position2D& comp) {
    glm::vec2 pos;
    if (dataVal.getValue(pos)) {
        comp = pos;
        return true;
    } else {
        return false;
    }
}