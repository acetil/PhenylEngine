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
        ShapeId deserialiseShape (const util::DataValue& val, ColliderId collider, std::size_t layers, std::size_t mask);
        util::DataValue serialiseCollider (ColliderId collider) const;
        ColliderId deserialiseCollider (const util::DataValue& val, component::EntityId entityId);

        void solveConstraints (std::vector<Constraint2D>& constraints, component::EntityComponentManager& compManager);
    public:
        void addComponentSerialisers(component::EntitySerialiser &serialiser) override;
        void updatePhysics(component::EntityComponentManager& componentManager, float deltaTime) override;
        void checkCollisions(component::EntityComponentManager& componentManager, const event::EventBus::SharedPtr &eventBus,
                             view::GameView &gameView, float deltaTime) override;

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