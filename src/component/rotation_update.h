#include "rotation_component.h"
#include "component.h"
#include "event/event.h"
#ifndef ROTATION_UPDATE_H
#define ROTATION_UPDATE_H
namespace component {
    void rotateEntity (int entityId, float newRotation, EntityComponentManager* manager, event::EventBus* eventBus);
    void rotateEntityBy (int entityId, float deltaTheta, EntityComponentManager* manager, event::EventBus* eventBus);
}
#endif
