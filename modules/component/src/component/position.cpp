#include "component/position.h"
#include "util/data.h"
#include "util/optional.h"

using namespace component;

util::DataValue component::serialisePos2D (const component::Position2D& pos2D) {
    return (util::DataValue)pos2D.get();
}

util::Optional<Position2D> component::deserialisePos2D (const util::DataValue& data) {
    glm::vec2 pos;
    if (data.getValue(pos)) {
        return {Position2D{pos}};
    } else {
        return util::NullOpt;
    }
}