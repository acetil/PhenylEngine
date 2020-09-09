#include "component/main_component.h"
#include "component/component.h"
#include "game/entity/entity.h"
#include "event/events/entity_collision.h"
#include "component/view/view.h"
#ifndef CONTROLLER_H
#define CONTROLLER_H
namespace game {
    class EntityController {
    public:
        virtual void controlEntityPrePhysics (view::EntityView& entityView);
        virtual void controlEntityPostPhysics (view::EntityView& entityView);
        virtual void onEntityCollision (view::EntityView& entityView, int otherEntityId, unsigned int layers);
    };
    void controlEntitiesPrePhysics (component::EntityComponentManager* manager, int startId,
                                    int numEntities, int direction);
    void controlEntitiesPostPhysics (component::EntityComponentManager* manager, int startId,
                                     int numEntities, int direction);
    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (event::EventBus* eventBus);
}
#endif