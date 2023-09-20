#include "physics_obj_2d.h"
#include "physics/components/2D/rigid_body.h"
#include "common/debug.h"

#include "common/components/2d/global_transform.h"
#include "physics/components/2D/rigid_body.h"
#include "physics/components/2D/colliders/box_collider.h"
#include "physics/signals/collision.h"

#include "common/components/2d/global_transform_serialize.h"
#include "physics/components/2D/colliders/box_collider_serialize.h"
#include "physics/components/2D/rigid_body.h"
#include "physics/components/2D/rigid_body_serialize.h"
#include "component/component_serializer.h"

#define SOLVER_ITERATIONS 10

using namespace phenyl::physics;

void PhysicsObject2D::addComponents (component::EntityComponentManager& componentManager) {
    componentManager.addComponent<RigidBody2D>();
    componentManager.addComponent<Collider2D>();
    componentManager.addComponent<BoxCollider2D>();

    componentManager.inherits<BoxCollider2D, Collider2D>();

    componentManager.addRequirement<RigidBody2D, common::GlobalTransform2D>();
    componentManager.addRequirement<BoxCollider2D, common::GlobalTransform2D>();
    componentManager.addRequirement<BoxCollider2D, RigidBody2D>();
}

void PhysicsObject2D::addComponentSerializers (component::EntitySerializer& serializer) {
    /*serialiser.addComponentSerialiser<RigidBody2D>("RigidBody2D", [] (const RigidBody2D& body) -> util::DataValue {
        return body.serialise();
    }, [] (const util::DataValue& val) -> util::Optional<RigidBody2D> {
        RigidBody2D body{};
        if (!body.deserialise(val)) {
            return util::NullOpt;
        }
        return {body};
    });
    serialiser.addComponentSerialiser<BoxCollider2D>("BoxCollider2D", [] (const BoxCollider2D& box) -> util::DataValue {
        return box.serialise();
    }, [] (const util::DataValue& val) -> util::Optional<BoxCollider2D> {
        BoxCollider2D box{};
        if (!box.deserialise(val)) {
            return util::NullOpt;
        }
        return {box};
    });*/
    serializer.addSerializer<RigidBody2D>();
    serializer.addSerializer<BoxCollider2D>();
}

void PhysicsObject2D::updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) {
    componentManager.query<common::GlobalTransform2D, RigidBody2D>().each([deltaTime] (component::Entity entity, common::GlobalTransform2D& transform, RigidBody2D& body) {
        body.doMotion(transform, deltaTime);
    });
}

void PhysicsObject2D::checkCollisions (component::EntityComponentManager& compManager, float deltaTime) {
    compManager.query<common::GlobalTransform2D, RigidBody2D, Collider2D>().each([] (auto info, const common::GlobalTransform2D& transform, const RigidBody2D& body, Collider2D& collider) {
        collider.syncUpdates(body, transform.transform2D.position());
    });

    compManager.query<common::GlobalTransform2D, BoxCollider2D>().each([] (auto info, const common::GlobalTransform2D& transform, BoxCollider2D& collider) {
        collider.applyFrameTransform(transform.transform2D.rotMatrix());
    });

    std::vector<std::tuple<component::EntityId, component::EntityId, std::uint32_t>> events;
    std::vector<Constraint2D> constraints;
    compManager.query<BoxCollider2D>().pairs([&constraints, &events, deltaTime] (component::QueryBundle<BoxCollider2D> boxBundle1, component::QueryBundle<BoxCollider2D> boxBundle2) {
        //auto [info1, box1] = boxBundle1;
        //auto [info2, box2] = boxBundle2;
        auto entity1 = boxBundle1.entity();
        auto entity2 = boxBundle2.entity();
        auto& box1 = boxBundle1.get<BoxCollider2D>();
        auto& box2 = boxBundle2.get<BoxCollider2D>();
        auto& manager = entity1.manager();
        if (!box1.shouldCollide(box2)) {
            return;
        }

        box1.collide(box2)
            .ifPresent([&] (SATResult2D result) {
                auto face1 = box1.getSignificantFace(result.normal);
                auto face2 = box2.getSignificantFace(-result.normal);

                auto manifold = buildManifold(face1, face2, result.normal, result.depth);
                constraints.push_back(manifold.buildConstraint(&box1, &box2, deltaTime));

                if (box1.layers & box2.mask) {
                    //manager._signal<OnCollision>(entity2.id(), entity1.id(), (std::uint32_t)(box1.layers & box2.mask));
                    entity2.signal(OnCollision{entity1.id(), (std::uint32_t)(box1.layers & box2.mask)});
                    //events.emplace_back(info2.id(), info1.id(), box1.layers & box2.mask);
                }

                if (box2.layers & box1.mask) {
                    //manager.signal<OnCollision>(entity1.id(), entity2.id(), (std::uint32_t)(box2.layers & box1.mask));
                    entity1.signal(OnCollision{entity2.id(), (std::uint32_t)(box2.layers & box1.mask)});
                    //events.emplace_back(info1.id(), info2.id(), box2.layers & box1.mask);
                }
            });
    });

    solveConstraints(constraints, compManager);

    /*for (const auto& [id1, id2, layers] : events) {
        //eventBus->raise(event::EntityCollisionEvent{compManager.entity(id1), compManager.entity(id2), layers, compManager, eventBus, gameView});
        compManager.signal<OnCollision>(id1, id2, layers);
    }*/
}

void PhysicsObject2D::solveConstraints (std::vector<Constraint2D>& constraints, component::EntityComponentManager& compManager) {
    //std::shuffle(constraints.begin(), constraints.end(), std::random_device{});
    for (auto i = 0; i < SOLVER_ITERATIONS; i++) {
        bool shouldContinue = false;

        for (auto& c : constraints) {
            auto res = c.solve();
            shouldContinue = shouldContinue || res;
        }

        if (!shouldContinue) {
            break;
        }
    }

    compManager.query<RigidBody2D, Collider2D>().each([] (auto info, RigidBody2D& body, const Collider2D& collider) {
        collider.updateBody(body);
    });
}

void PhysicsObject2D::debugRender (const component::EntityComponentManager& componentManager) {
    // Debug render
    componentManager.query<common::GlobalTransform2D, BoxCollider2D>().each([] (component::ConstEntity entity, const common::GlobalTransform2D& transform, const BoxCollider2D& box) {
        auto pos1 = box.frameTransform * glm::vec2{-1, -1} + transform.transform2D.position();
        auto pos2 = box.frameTransform * glm::vec2{1, -1} + transform.transform2D.position();
        auto pos3 = box.frameTransform * glm::vec2{1, 1} + transform.transform2D.position();
        auto pos4 = box.frameTransform * glm::vec2{-1, 1} + transform.transform2D.position();

        common::debugWorldRectOutline(pos1, pos2, pos3, pos4, {0, 0, 1, 1});
    });
}