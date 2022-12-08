#pragma once

#include "component/view/game_view.h"
#include "component/component.h"
#include "component/forward.h"
#include "physics/components/simple_friction.h"
#include "common/events/entity_creation.h"
#include "common/events/entity_rotation.h"
#include "event/event_bus.h"

#include "physics_core.h"
#include "shape/physics_shape.h"

namespace physics {

    struct ColliderTag {};
    struct ShapeTag {};
    using ColliderId = PhysicsId<ColliderTag, 0, 0>;
    using ShapeId = PhysicsId<ShapeTag, 0, 0>;



    class IPhysics {
    protected:
        template <PhysicsIdType T>
        T makeId (std::uint64_t index) const requires (!util::GameIdHasGen<T> && !util::GameIdHasType<T>){
            return T{index};
        }

        template <PhysicsIdType T>
        std::uint64_t getIdIndex (T id) const {
            return id.getIndex();
        }

    public:
        virtual ~IPhysics() = default;
        virtual void addComponentSerialisers (component::EntitySerialiser& serialiser) = 0;
        virtual void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) = 0;
        virtual void checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView) = 0;

        virtual ColliderId addCollider (component::EntityId entityId) = 0;
        virtual void destroyCollider (ColliderId id) = 0;

        virtual ShapeId getColliderHitbox (ColliderId id) = 0;
        virtual ShapeId getColliderEventbox (ColliderId id) = 0;
        virtual bool colliderShapesMerged (ColliderId id) = 0;
        virtual void setColliderShapesMerged (ColliderId id, bool merged) = 0;

        virtual void setShapeType (ShapeId id, PrimitiveShape shape) = 0;
        virtual ShapeData getShapeData (ShapeId id) = 0;
    };

    std::unique_ptr<IPhysics> makeDefaultPhysics ();
}
