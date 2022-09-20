#include <math.h>
#include <limits>

#include "component/components/2D/rotation.h"
#include "util/data.h"
//#include "event/events/entity_rotation.h"

using namespace component;

/*void rotateEntityBy2 (EntityId entityId, Rotation2D& comp, float deltaTheta, const EntityComponentManager::SharedPtr& manager, const event::EventBus::SharedPtr& eventBus) {
    auto oldMat = comp.rotMatrix;
    auto rotMat = glm::mat2{{cos(deltaTheta), -1.0f * sin(deltaTheta)}, {sin(deltaTheta), cos(deltaTheta)}};
    comp.rotMatrix = oldMat * rotMat;
    comp.rotation += deltaTheta;
    //eventBus->raiseEvent(event::EntityRotationEvent(entityId, manager, oldMat, rotMat));
}

void component::rotateEntity (EntityId entityId, float newRotation, EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& eventBus) {
    //auto component = manager->getObjectDataPtr<Rotation2D>(entityId);
    manager->getObjectData<Rotation2D>(entityId).ifPresent([entityId, newRotation, &manager, eventBus](Rotation2D& comp) {
       if (abs(newRotation - comp.rotation) > std::numeric_limits<float>::epsilon()) {
           rotateEntityBy2(entityId, comp, newRotation - comp.rotation, manager, eventBus);
       }
    });
    if (abs(newRotation - component->rotation) > std::numeric_limits<float>::epsilon()) {
        rotateEntityBy(entityId, newRotation - component->rotation, manager, eventBus);
    }
}
void component::rotateEntityBy (EntityId entityId, float deltaTheta, const EntityComponentManager::SharedPtr& manager, const event::EventBus::SharedPtr& eventBus) {
    /*auto component = manager->getObjectDataPtr<Rotation2D>(entityId);
    glm::mat2 oldMat = component->rotMatrix;
    auto rotMat = glm::mat2({{cos(deltaTheta), -1.0f * sin(deltaTheta)}, {sin(deltaTheta), cos(deltaTheta)}});
    component->rotMatrix = oldMat * rotMat;
    eventBus->raiseEvent(event::EntityRotationEvent(entityId, manager, oldMat, rotMat));
    component->rotation += deltaTheta;
    manager->getObjectData<Rotation2D>(entityId).ifPresent([entityId, deltaTheta, &manager, eventBus](Rotation2D& comp) {
        rotateEntityBy2(entityId, comp, deltaTheta, manager, eventBus);
    });
}*/

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
