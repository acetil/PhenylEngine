#pragma once

#include "physics/physics.h"
#include "collider_2d.h"
#include "util/fl_vector.h"


namespace physics {
    class PhysicsObject2D : public IPhysics {
    private:
        util::FLVector<Collider2D> colliders;


        bool colliderExists (ColliderId id) const;
        Collider2D& getCollider (ColliderId id);
        const Collider2D& getCollider (ColliderId id) const;

    public:
        void addComponentSerialisers(component::EntitySerialiser &serialiser) override;
        void updatePhysics(const component::EntityComponentManager::SharedPtr &componentManager) override;
        void checkCollisions(const component::EntityComponentManager::SharedPtr &componentManager, const event::EventBus::SharedPtr &eventBus,
                             view::GameView &gameView) override;

        ColliderId addCollider(component::EntityId entityId) override;
        void destroyCollider(physics::ColliderId id) override;

        ShapeId getColliderHitbox(physics::ColliderId id) override;
        ShapeId getColliderEventbox(physics::ColliderId id) override;
        bool colliderShapesMerged(physics::ColliderId id) override;
        void setColliderShapesMerged(physics::ColliderId id, bool merged) override;


        void setShapeType(physics::ShapeId id, physics::PrimitiveShape shape) override;
        ShapeData getShapeData(physics::ShapeId id) override;
    };
}