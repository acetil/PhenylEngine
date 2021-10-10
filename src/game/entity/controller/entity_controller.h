#include "component/main_component.h"
#include "component/component.h"
#include "game/entity/entity.h"
#include "event/events/entity_collision.h"
#include "component/view/view.h"
#include "component/view/game_view.h"
#include "util/data.h"

#ifndef CONTROLLER_H
#define CONTROLLER_H
namespace game {
    class EntityController {
    private:
        int testTexId = 0;
    public:
        virtual void controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView);
        virtual void controlEntityPostPhysics (view::EntityView& entityView, view::GameView& gameView);
        virtual void onEntityCollision (view::EntityView& entityView, view::GameView& gameView, int otherEntityId, unsigned int layers); // TODO: hit/hurtbox
        virtual int getTextureId (view::EntityView& entityView, view::GameView& gameView);
        virtual void setTextureIds (graphics::TextureAtlas& atlas);
        virtual void initEntity (view::EntityView& entityView, view::GameView&) {};
        virtual void initEntity (view::EntityView& entityView, view::GameView&, const util::DataValue& data) {}; // TODO: convert to binary format
        virtual util::DataObject getData (view::EntityView& entityView, view::GameView& gameView);

    };
    void controlEntitiesPrePhysics (component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId,
                                    int numEntities, int direction, const event::EventBus::SharedPtr& bus);
    void controlEntitiesPostPhysics (component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId,
                                     int numEntities, int direction, const event::EventBus::SharedPtr& bus);
    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (const event::EventBus::SharedPtr& eventBus);
}
#endif