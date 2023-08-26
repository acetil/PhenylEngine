#pragma once

#include "component/view/game_view.h"
#include "component/component.h"
#include "component/forward.h"
#include "physics/components/simple_friction.h"
#include "common/events/entity_creation.h"
#include "event/event_bus.h"

#include "physics_core.h"
#include "shape/physics_shape.h"

namespace physics {

    struct ColliderTag {};
    struct ShapeTag {};
    using ColliderId = PhysicsId<ColliderTag, 0, 0>;
    using ShapeId = PhysicsId<ShapeTag, 8, 1>;



    class IPhysics {
    protected:
        /*template <PhysicsIdType T>
        T makeId (std::uint64_t index) const requires (!util::GameIdHasGen<T> && !util::GameIdHasType<T>){
            return T{index};
        }*/

        template <PhysicsIdType T, typename ...Args>
        T makeId (Args&&... args) const {
            return T{std::forward<Args>(args)...};
        }

        template <PhysicsIdType T>
        std::uint64_t getIdIndex (T id) const {
            return id.getIndex();
        }

        template <PhysicsIdType T>
        auto makePublicId (T id) const {
            return id.convert();
        }

        virtual ShapeDataNew getShapeData (ShapeId id) const = 0;
        virtual ShapeId makeNewHitbox (ColliderId collider, std::size_t typeIndex, void* request) = 0;
        //virtual ShapeId makeNewEventbox (ColliderId collider, std::size_t typeIndex, void* request) = 0;

    public:
        virtual ~IPhysics() = default;
        virtual void addComponentSerialisers (component::EntitySerialiser& serialiser) = 0;
        virtual void updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) = 0;
        virtual void checkCollisions (component::EntityComponentManager& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView) = 0;

        virtual ColliderId addCollider (component::EntityId entityId) = 0;
        virtual void destroyCollider (ColliderId id) = 0;

        virtual ShapeId getColliderHitbox (ColliderId id) = 0;
        //virtual ShapeId getColliderEventbox (ColliderId entityId) = 0;
        //virtual bool colliderShapesMerged (ColliderId entityId) = 0;
        //virtual void setColliderShapesMerged (ColliderId entityId, bool merged) = 0;

        virtual void setShapeType (ShapeId id, PrimitiveShape shape) = 0;
        virtual void addEventHandlers (const event::EventBus::SharedPtr& eventBus) = 0;
        virtual void debugRender (const component::EntityComponentManager& componentManager) = 0;
       // virtual ShapeData getShapeData (ShapeId entityId) = 0;

        template <typename T>
        ShapeView<T> getShape (ShapeId id) {
            return getShapeData(id).as<T>();
        }

        template <typename T>
        ShapeId makeHitbox (ColliderId id, ShapeRequest<T> request) {
            return makeNewHitbox(id, ShapeRequest<T>::shapeType, static_cast<void*>(&request));
        }

        /*template <typename T>
        ShapeId makeEventbox (ColliderId entityId, ShapeRequest<T> request) {
            return makeNewEventbox(entityId, ShapeRequest<T>::shapeType, static_cast<void*>(&request));
        }*/
    };

    std::unique_ptr<IPhysics> makeDefaultPhysics ();
}
