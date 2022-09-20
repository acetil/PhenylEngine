#include "component/components/2D/position.h"
#include "util/data.h"
#include "util/optional.h"

using namespace component;

/*util::DataValue component::serialisePos2D (const component::Position2D& pos2D) {
    return (util::DataValue)pos2D.get();
}*/

util::DataValue component::phenyl_to_data (const component::Position2D& comp) {
    return (util::DataValue)comp.get();
}

/*util::Optional<Position2D> component::deserialisePos2D (const util::DataValue& data) {
    glm::vec2 pos;
    if (data.getValue(pos)) {
        return {Position2D{pos}};
    } else {
        return util::NullOpt;
    }
}*/

bool component::phenyl_from_data (const util::DataValue& dataVal, component::Position2D& comp) {
    glm::vec2 pos;
    if (dataVal.getValue(pos)) {
        comp = pos;
        return true;
    } else {
        return false;
    }
}