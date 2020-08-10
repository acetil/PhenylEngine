#include "component/main_component.h"
#include "component/component.h"
#include "game/entity/entity.h"
#include "event/events/entity_collision.h"
#ifndef CONTROLLER_H
#define CONTROLLER_H
namespace game {
    class EntityController {
        public:
        virtual void controlEntityPrePhysics (AbstractEntity* entity, component::EntityMainComponent *comp, int entityId,
                                               component::EntityComponentManager* manager);
        virtual void controlEntityPostPhysics (AbstractEntity* entity, component::EntityMainComponent* comp, int entityId,
                                                component::EntityComponentManager* manager);
        virtual void onEntityCollision (AbstractEntity* entity, int entityId, AbstractEntity* other, int otherEntityId,
                                        component::EntityComponentManager* manager, unsigned int layers);
    };
    void controlEntitiesPrePhysics (AbstractEntity** entities, component::EntityMainComponent* comp, int startId,
                                    int numEntities, int direction, component::EntityComponentManager* manager);
    void controlEntitiesPostPhysics (AbstractEntity** entities, component::EntityMainComponent* comp,int startId,
                                     int numEntities, int direction, component::EntityComponentManager* manager);
    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (event::EventBus* eventBus);
}
#endif