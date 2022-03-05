#pragma once

#include "component/main_component.h"
#include "component/component.h"
#include "engine/entity/entity.h"
#include "event/events/entity_collision.h"
#include "component/view/view.h"
#include "component/view/game_view.h"
#include "util/data.h"

namespace game {
    class EntityController {
    private:
        int testTexId = 0;
    public:
        virtual void controlEntityPrePhysics (component::view::EntityView& entityView, view::GameView& gameView);
        virtual void controlEntityPostPhysics (component::view::EntityView& entityView, view::GameView& gameView);
        virtual void onEntityCollision (component::view::EntityView& entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers); // TODO: hit/hurtbox
        virtual int getTextureId (component::view::EntityView& entityView, view::GameView& gameView) const;
        virtual void setTextureIds (graphics::TextureAtlas& atlas);
        virtual void initEntity (component::view::EntityView& entityView, view::GameView&) {};
        virtual void initEntity (component::view::EntityView& entityView, view::GameView&, const util::DataValue& data) {}; // TODO: convert to binary format
        virtual util::DataObject getData (component::view::EntityView& entityView, view::GameView& gameView);

    };
    /*void controlEntitiesPrePhysics (component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId,
                                    int numEntities, int direction, const event::EventBus::SharedPtr& bus);*/

    void controlEntitiesPrePhysics (const component::EntityComponentManager::SharedPtr& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus);

    /*void controlEntitiesPostPhysics (component::EntityComponentManager::SharedPtr manager, view::GameView& gameView, int startId,
                                     int numEntities, int direction, const event::EventBus::SharedPtr& bus);*/

    void controlEntitiesPostPhysics (const component::EntityComponentManager::SharedPtr& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus);

    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (const event::EventBus::SharedPtr& eventBus);
}
