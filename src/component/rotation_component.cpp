#include <math.h>
#include <limits>

#include "rotation_component.h"
#include "rotation_update.h"
#include "event/events/entity_rotation.h"

using namespace component;

void component::rotateEntity (int entityId, float newRotation, EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& eventBus) {
    auto component = manager->getObjectDataPtr<RotationComponent>(entityId);
    if (abs(newRotation - component->rotation) > std::numeric_limits<float>::epsilon()) {
        rotateEntityBy(entityId, newRotation - component->rotation, manager, eventBus);
    }
}
void component::rotateEntityBy (int entityId, float deltaTheta, EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& eventBus) {
    auto component = manager->getObjectDataPtr<RotationComponent>(entityId);
    glm::mat2 oldMat = component->rotMatrix;
    auto rotMat = glm::mat2({{cos(deltaTheta), -1.0f * sin(deltaTheta)}, {sin(deltaTheta), cos(deltaTheta)}});
    component->rotMatrix = oldMat * rotMat;
    eventBus->raiseEvent(event::EntityRotationEvent(entityId, manager, oldMat, rotMat));
    component->rotation += deltaTheta;
}

util::DataValue RotationComponent::serialise () const {
    return (util::DataValue)rotation;
}

void RotationComponent::deserialise (const util::DataValue& val) {
    rotation = val.get<float>();
}
