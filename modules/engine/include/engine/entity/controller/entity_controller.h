#pragma once

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
        std::string entityId;
    public:
        explicit EntityController (std::string entityId);
        virtual void controlEntityPrePhysics (component::Entity& entityView, view::GameView& gameView);
        virtual void controlEntityPostPhysics (component::Entity& entityView, view::GameView& gameView);
        virtual void onEntityCollision (component::Entity& entityView, view::GameView& gameView, component::Entity& otherEntity, unsigned int layers); // TODO: hit/hurtbox
        virtual void initEntity (component::Entity& entityView, view::GameView&) {};
        virtual void initEntity (component::Entity& entityView, view::GameView&, const util::DataValue& data) {}; // TODO: convert to binary format
        virtual util::DataObject getData (component::Entity& entityView, view::GameView& gameView);
        [[nodiscard]] const std::string& getEntityId () const;
    };

    struct EntityControllerComponent {
    private:
        EntityController* controller;
        explicit EntityControllerComponent (EntityController* controller);
    public:
        EntityController& get ();
        [[nodiscard]] const EntityController& get () const;

        friend class detail::GameObject;
    };
    void controlEntitiesPrePhysics (component::EntityComponentManager& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus);
    void controlEntitiesPostPhysics (component::EntityComponentManager& manager, view::GameView& gameView, const event::EventBus::SharedPtr& bus);

    void controlOnCollision (event::EntityCollisionEvent& event);
    void addControlEventHandlers (const event::EventBus::SharedPtr& eventBus);


}
