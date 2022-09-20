#include "component/components/2D/rotation.h"
#include "util/data.h"

using namespace component;

util::DataValue component::phenyl_to_data (const component::Rotation2D& comp) {
    return (util::DataValue)comp.rotation;
}

bool component::phenyl_from_data (const util::DataValue& dataVal, component::Rotation2D& comp) {
    if (!dataVal.is<float>()) {
        return false;
    }

    comp = dataVal.get<float>();

    return true;
}

Rotation2D& Rotation2D::operator= (float newRot) {
    rotation = newRot;
    rotMatrix = {{glm::cos(newRot), glm::sin(newRot)}, {-glm::sin(newRot), glm::cos(newRot)}};
    return *this;
}
