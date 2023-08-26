#pragma once

#include "physics/physics.h"
#include "collider_2d.h"
#include "util/fl_vector.h"
#include "shape_registry_2d.h"


namespace physics {
    class PhysicsObject2D : public IPhysics {
    private:
        util::FLVector<Collider2D> colliders;
        ShapeRegistry2D shapeRegistry;
        event::EventScope scope;
        bool debugColliderRender = false;

        ShapeId makeShapeFromRequest (ShapeRegistry2D& registry, ColliderId collider, std::size_t typeIndex, void* request);

        bool colliderExists (ColliderId id) const;
        Collider2D& getCollider (ColliderId id);
        const Collider2D& getCollider (ColliderId id) const;
        void resolveCollision (ColliderId id1, ColliderId id2, glm::vec2 disp, component::EntityComponentManager& compManager);
        ShapeId deserialiseShape (const util::DataValue& val, ColliderId collider, std::size_t layers, std::size_t mask);
    public:
        void addComponentSerialisers(component::EntitySerialiser &serialiser) override;
        void updatePhysics(component::EntityComponentManager& componentManager) override;
        void checkCollisions(component::EntityComponentManager& componentManager, const event::EventBus::SharedPtr &eventBus,
                             view::GameView &gameView) override;

        void checkCollisionsNew (const component::EntityComponentManager& compManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView);

        ColliderId addCollider(component::EntityId entityId) override;
        void destroyCollider(physics::ColliderId id) override;

        ShapeId getColliderHitbox(physics::ColliderId id) override;
        //ShapeId getColliderEventbox(physics::ColliderId entityId) override;
        //bool colliderShapesMerged(physics::ColliderId entityId) override;
        //void setColliderShapesMerged(physics::ColliderId entityId, bool merged) override;


        void setShapeType(physics::ShapeId id, physics::PrimitiveShape shape) override;
        ShapeDataNew getShapeData(physics::ShapeId id) const override;

        ShapeId makeNewHitbox(physics::ColliderId colliderId, std::size_t typeIndex, void *request) override;
        //ShapeId makeNewEventbox(physics::ColliderId collider, std::size_t typeIndex, void *request) override;

        void addEventHandlers (const event::EventBus::SharedPtr& eventBus) override;
        void debugRender (const component::EntityComponentManager& componentManager) override;
    };
}