#pragma once

#include "component/main_component.h"
#include "component/component.h"
//#include "engine/entity/entity.h"
#include "common/events/entity_collision.h"
#include "component/view/game_view.h"
#include "util/data.h"

namespace graphics {
    class TextureAtlas;
}

namespace game {
    class EntityController {
    private:
        //int testTexId = 0;
    public:
        virtual void controlEntityPrePhysics (component::EntityView& entityView, view::GameView& gameView);
        virtual void controlEntityPostPhysics (component::EntityView& entityView, view::GameView& gameView);
        virtual void onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers); // TODO: hit/hurtbox
        //virtual int getTextureId (component::EntityView& entityView, view::GameView& gameView) const;
        //virtual void setTextureIds (graphics::TextureAtlas& atlas);
        virtual void initEntity (component::EntityView& entityView, view::GameView&) {};
        virtual void initEntity (component::EntityView& entityView, view::GameView&, const util::DataValue& data) {}; // TODO: convert to binary format
        virtual util::DataObject getData (component::EntityView& entityView, view::GameView& gameView);

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
