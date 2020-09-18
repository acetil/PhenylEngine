#include "component/main_component.h"
#include "component/component.h"
#include "game/entity/entity.h"
#include "event/events/entity_collision.h"
#include "component/view/view.h"
#include "component/view/game_view.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H
namespace game {
    class EntityController {
    public:
        virtual void controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView);
        virtual void controlEntityPostPhysics (view::EntityView& entityView, view::GameView& gameView);
        virtual void onEntityCollision (view::EntityView& entityView, view::GameView& gameView, int otherEntityId, unsigned int layers);
    };
    void controlEntitiesPrePhysics (component::EntityComponentManager* manager, view::GameView& gameView, int startId,
                                    int numEntities, int direction, event::EventBus* bus);
    void controlEntitiesPostPhysics (component::EntityComponentManager* manager, view::GameView& gameView, int startId,
                                     int numEntities, int direction, event::EventBus* bus);
    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (event::EventBus* eventBus);
}
#endif