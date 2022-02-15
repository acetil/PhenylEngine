#include <math.h>
#include <limits>

#include "component/rotation_component.h"
#include "component/rotation_update.h"
#include "event/events/entity_rotation.h"

using namespace component;

void rotateEntityBy2 (EntityId entityId, RotationComponent& comp, float deltaTheta, const EntityComponentManager::SharedPtr& manager, const event::EventBus::SharedPtr& eventBus) {
    auto oldMat = comp.rotMatrix;
    auto rotMat = glm::mat2{{cos(deltaTheta), -1.0f * sin(deltaTheta)}, {sin(deltaTheta), cos(deltaTheta)}};
    comp.rotMatrix = oldMat * rotMat;
    comp.rotation += deltaTheta;
    eventBus->raiseEvent(event::EntityRotationEvent(entityId, manager, oldMat, rotMat));
}

void component::rotateEntity (EntityId entityId, float newRotation, EntityComponentManager::SharedPtr manager, const event::EventBus::SharedPtr& eventBus) {
    //auto component = manager->getObjectDataPtr<RotationComponent>(entityId);
    manager->getObjectData<RotationComponent>(entityId).ifPresent([entityId, newRotation, &manager, eventBus](RotationComponent& comp) {
       if (abs(newRotation - comp.rotation) > std::numeric_limits<float>::epsilon()) {
           rotateEntityBy2(entityId, comp, newRotation - comp.rotation, manager, eventBus);
       }
    });
    /*if (abs(newRotation - component->rotation) > std::numeric_limits<float>::epsilon()) {
        rotateEntityBy(entityId, newRotation - component->rotation, manager, eventBus);
    }*/
}
void component::rotateEntityBy (EntityId entityId, float deltaTheta, const EntityComponentManager::SharedPtr& manager, const event::EventBus::SharedPtr& eventBus) {
    /*auto component = manager->getObjectDataPtr<RotationComponent>(entityId);
    glm::mat2 oldMat = component->rotMatrix;
    auto rotMat = glm::mat2({{cos(deltaTheta), -1.0f * sin(deltaTheta)}, {sin(deltaTheta), cos(deltaTheta)}});
    component->rotMatrix = oldMat * rotMat;
    eventBus->raiseEvent(event::EntityRotationEvent(entityId, manager, oldMat, rotMat));
    component->rotation += deltaTheta;*/
    manager->getObjectData<RotationComponent>(entityId).ifPresent([entityId, deltaTheta, &manager, eventBus](RotationComponent& comp) {
        rotateEntityBy2(entityId, comp, deltaTheta, manager, eventBus);
    });
}

util::DataValue RotationComponent::serialise () const {
    return (util::DataValue)rotation;
}

void RotationComponent::deserialise (const util::DataValue& val) {
    rotation = val.get<float>();
}
