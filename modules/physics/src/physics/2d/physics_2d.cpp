#include "physics_2d.h"

#include "core/clock.h"
#include "core/components/2d/global_transform.h"
#include "core/debug.h"
#include "core/runtime.h"
#include "core/serialization/component_serializer.h"
#include "physics/2d/collisions_2d.h"
#include "physics/components/2D/colliders/box_collider.h"
#include "physics/components/2D/rigid_body.h"
#include "physics/signals/collision.h"

#define SOLVER_ITERATIONS 10

using namespace phenyl::physics;

struct Constraints2D : public phenyl::core::IResource {
    std::vector<Constraint2D> constraints;

    std::string_view getName () const noexcept override {
        return "Constraints2D";
    }
};

static void RigidBody2DMotionSystem (const phenyl::core::Resources<const phenyl::core::Clock>& resources,
    phenyl::core::Transform2D& transform, RigidBody2D& body) {
    auto& [clock] = resources;

    body.doMotion(transform, static_cast<float>(clock.deltaTime()));
}

static void Collider2DSyncSystem (const phenyl::core::GlobalTransform2D& transform, const RigidBody2D& body,
    Collider2D& collider) {
    collider.syncUpdates(body, transform.position());
}

static void BoxCollider2DFrameTransformSystem (const phenyl::core::GlobalTransform2D& transform,
    BoxCollider2D& collider) {
    collider.applyFrameTransform(transform.transform.linearTransform());
}

static void CollisionCheck2DSystem (const phenyl::core::Resources<Constraints2D, const phenyl::core::Clock>& resources,
    const phenyl::core::Bundle<BoxCollider2D>& bundle1, const phenyl::core::Bundle<BoxCollider2D>& bundle2) {
    auto& [constraints, clock] = resources;

    auto entity1 = bundle1.entity();
    auto entity2 = bundle2.entity();
    auto& box1 = bundle1.get<BoxCollider2D>();
    auto& box2 = bundle2.get<BoxCollider2D>();
    if (!box1.shouldCollide(box2)) {
        return;
    }

    auto collision = box1.collide(box2);
    if (collision) {
        auto result = *collision;
        auto face1 = box1.getSignificantFace(result.normal);
        auto face2 = box2.getSignificantFace(-result.normal);

        auto manifold = buildManifold(face1, face2, result.normal, result.depth);
        constraints.constraints.emplace_back(manifold.buildConstraint(&box1, &box2, clock.deltaTime()));

        auto contactPoint = manifold.getContactPoint();
        if (box1.layers & box2.mask) {
            // manager._signal<OnCollision>(entity2.id(), entity1.id(),
            // (std::uint32_t)(box1.layers & box2.mask));
            entity2.raise(
                OnCollision{entity1.id(), (std::uint32_t) (box1.layers & box2.mask), contactPoint, -result.normal});
            // events.emplace_back(info2.id(), info1.id(), box1.layers & box2.mask);
        }

        if (box2.layers & box1.mask) {
            // manager.signal<OnCollision>(entity1.id(), entity2.id(),
            // (std::uint32_t)(box2.layers & box1.mask));
            entity1.raise(
                OnCollision{entity2.id(), (std::uint32_t) (box2.layers & box1.mask), contactPoint, result.normal});
            // events.emplace_back(info1.id(), info2.id(), box2.layers & box1.mask);
        }
    }
}

static void Constraints2DSolveSystem (const phenyl::core::Resources<Constraints2D>& resources) {
    auto& [constraints] = resources;

    for (auto i = 0; i < SOLVER_ITERATIONS; i++) {
        bool shouldContinue = false;

        for (auto& c : constraints.constraints) {
            auto res = c.solve();
            shouldContinue = shouldContinue || res;
        }

        if (!shouldContinue) {
            break;
        }
    }

    constraints.constraints.clear();
}

static void Collider2DUpdateSystem (RigidBody2D& body, const Collider2D& collider) {
    collider.updateBody(body);
}

void Physics2D::addComponents (core::PhenylRuntime& runtime) {
    runtime.addComponent<RigidBody2D>("RigidBody2D");
    // runtime.addUnserializedComponent<Collider2D>("Collider2D");
    runtime.addComponent<BoxCollider2D>("BoxCollider2D");
    runtime.declareInterface<Collider2D, BoxCollider2D>();

    // runtime.manager().inherits<BoxCollider2D, Collider2D>();

    // runtime.manager().addRequirement<RigidBody2D, common::GlobalTransform2D>();
    // runtime.manager().addRequirement<BoxCollider2D, common::GlobalTransform2D>();
    // runtime.manager().addRequirement<BoxCollider2D, RigidBody2D>();

    runtime.addResource<Constraints2D>();
    auto& motionSystem = runtime.addSystem<core::PhysicsUpdate>("RigidBody2D::Update", RigidBody2DMotionSystem);

    auto& propagateSystem = runtime.addHierarchicalSystem<core::PhysicsUpdate>("Physics2D::PropagateTransforms",
        &core::GlobalTransform2D::PropagateTransforms);

    auto& syncSystem = runtime.addSystem<core::PhysicsUpdate>("Collider2D::Sync", Collider2DSyncSystem);
    auto& boxTransformSystem =
        runtime.addSystem<core::PhysicsUpdate>("BoxCollider2D::FrameTransform", BoxCollider2DFrameTransformSystem);
    auto& collCheckSystem = runtime.addSystem<core::PhysicsUpdate>("Physics2D::CollisionCheck", CollisionCheck2DSystem);
    auto& constraintSolveSystem =
        runtime.addSystem<core::PhysicsUpdate>("Physics2D::ConstraintsSolve", Constraints2DSolveSystem);
    auto& collUpdateSystem =
        runtime.addSystem<core::PhysicsUpdate>("Collider2D::PostCollision", Collider2DUpdateSystem);
    auto& propagateSystemEnd = runtime.addHierarchicalSystem<core::PhysicsUpdate>("Physics2D::PropagateTransformsEnd",
        &core::GlobalTransform2D::PropagateTransforms);

    motionSystem.runBefore(propagateSystem);
    propagateSystem.runBefore(syncSystem);
    syncSystem.runBefore(boxTransformSystem);
    boxTransformSystem.runBefore(collCheckSystem);
    collCheckSystem.runBefore(constraintSolveSystem);
    constraintSolveSystem.runBefore(collUpdateSystem);
    collUpdateSystem.runBefore(propagateSystemEnd);
}

void Physics2D::debugRender (core::World& world, core::Debug& debug) {
    // Debug render
    world.query<core::GlobalTransform2D, BoxCollider2D>().each(
        [&debug] (const core::GlobalTransform2D& transform, const BoxCollider2D& box) {
            // auto pos1 = box.m_frameTransform * glm::vec2{-1, -1} + transform.position();
            // auto pos2 = box.m_frameTransform * glm::vec2{1, -1} + transform.position();
            // auto pos3 = box.m_frameTransform * glm::vec2{1, 1} + transform.position();
            // auto pos4 = box.m_frameTransform * glm::vec2{-1, 1} + transform.position();
            auto start = box.m_frameTransform * glm::vec2{-1, -1} + transform.position();
            auto widthVec = box.m_frameTransform * glm::vec2{2, 0};
            auto heightVec = box.m_frameTransform * glm::vec2{0, 2};

            // core::debugWorldRectOutline(pos1, pos2, pos3, pos4, {0, 0, 1, 1});
            debug.displayWorldRect(core::DebugRect::Create(start, widthVec, heightVec), glm::vec4{0, 0, 1, 1}, true);
        });
}
