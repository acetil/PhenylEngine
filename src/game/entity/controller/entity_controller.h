#include "component/main_component.h"
#include "component/component.h"
#include "game/entity/entity.h"
#ifndef CONTROLLER_H
#define CONTROLLER_H
namespace game {
    class EntityController {
        public:
        virtual void controlEntityPrePhysics (AbstractEntity* entity, component::EntityMainComponent *comp, int entityId,
                                               component::ComponentManager* manager);
        virtual void controlEntityPostPhysics (AbstractEntity* entity, component::EntityMainComponent* comp, int entityId,
                                                component::ComponentManager* manager);
        virtual void onEntityCollision (AbstractEntity* entity, int entityId, AbstractEntity* other, int otherEntityId,
                                        component::ComponentManager* manager);
    };
}
#endif