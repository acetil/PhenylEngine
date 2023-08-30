#include <random>

#include "physics_obj_2d.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/rigid_body.h"
#include "common/events/entity_collision.h"
#include "common/events/entity_creation.h"
#include "common/debug.h"

#include "physics/shape/2d/box_shape_2d_interface.h"
#include "common/components/2d/global_transform.h"
#include "common/events/debug/debug_render.h"

#define SOLVER_ITERATIONS 10

using namespace physics;

ShapeId PhysicsObject2D::makeShapeFromRequest (ShapeRegistry2D& registry, ColliderId collider, std::size_t typeIndex, void* request) {
    switch (typeIndex) {
        case ShapeRequest<BoxShape2D>::type: {
            return makeId<ShapeId>(registry.addComponent<BoxShape2D>(static_cast<ShapeRequest<BoxShape2D>*>(request)->make(collider)));
        }

        default:
            logging::log(LEVEL_ERROR, "Unknown 2D shape type: {} requested for collider {}", typeIndex, collider.getValue());
            return {};
    }
}

ShapeId PhysicsObject2D::deserialiseShape (const util::DataValue& val, ColliderId collider, std::size_t layers, std::size_t mask) {
    if (!val.is<util::DataObject>()) {
        return {};
    }

    const auto& obj = val.get<util::DataObject>();
    if (!obj.contains("type") || !obj.at("type").is<std::string>()) {
        return {};
    }

    const auto& type = obj.at("type").get<std::string>();

    if (type == "BoxShape2D") {
        auto boxOpt = BoxShape2D::deserialise(obj, collider, layers, mask);
        if (!boxOpt) {
            return {};
        }

        return makeId<ShapeId>(shapeRegistry.addComponent<BoxShape2D>(boxOpt.getUnsafe()));
    } else {
        logging::log(LEVEL_ERROR, "Unknown shape type: {}", type);
        return {};
    }
}

util::DataValue PhysicsObject2D::serialiseCollider (ColliderId id) const {
    util::DataObject obj;
    auto& collider = getCollider(id);
    obj["layers"] = (unsigned int)collider.hitboxLayers;
    obj["masks"] = (unsigned int)collider.eventboxMask;
    obj["elasticity"] = (unsigned int)collider.elasticity;

    obj["shape"] = ((Shape2D*)shapeRegistry.getComponentErased(makePublicId(collider.hitbox)))->serialise();


    return obj;
}

ColliderId PhysicsObject2D::deserialiseCollider (const util::DataValue& val, component::EntityId entityId) {
    if (!val.is<util::DataObject>()) {
        return {};
    }
    const auto& obj = val.get<util::DataObject>();

    Collider2D coll{entityId};

    if (!obj.contains<int>("layers")) {
        return {};
    }
    coll.hitboxLayers = obj.at<unsigned int>("layers");

    if (!obj.contains<int>("masks")) {
        return {};
    }
    coll.eventboxMask = obj.at<unsigned int>("masks");

    if (obj.contains("elasticity")) {
        coll.elasticity = obj.at<float>("elasticity");
    }

    if (!obj.contains("shape")) {
        return {};
    }

    auto collId = makeId<ColliderId>(colliders.push(coll));
    auto shapeId = deserialiseShape(obj.at("shape"), collId, coll.hitboxLayers, coll.eventboxMask);
    if (!shapeId) {
        destroyCollider(collId);
        return {};
    }
    getCollider(collId).hitbox = shapeId;

    return collId;
}

void PhysicsObject2D::addComponentSerialisers (component::EntitySerialiser& serialiser) {
    serialiser.addComponentSerialiser<RigidBody2D>("RigidBody2D", [this] (const RigidBody2D& comp) -> util::DataValue {
        util::DataValue val = phenyl_to_data(comp);

        if (!val.is<util::DataObject>()) {
            logging::log(LEVEL_ERROR, "RigidBody2D serialise did not return DataObject!");
        }

        auto& obj = val.get<util::DataObject>();
        obj["collider"] = serialiseCollider(comp.getCollider());

        return val;
    }, [this] (const util::DataValue& val, component::EntityId id) -> util::Optional<RigidBody2D> {
        if (!val.is<util::DataObject>()) {
            logging::log(LEVEL_WARNING, "Serialised RigidBody2D is not DataObject!");
            return util::NullOpt;
        }

        const auto& obj = val.get<util::DataObject>();
        if (!obj.contains("collider")) {
            logging::log(LEVEL_WARNING, "Missing required collider attribute in serialised RigidBody2D!");
            return util::NullOpt;
        }

        auto collId = deserialiseCollider(obj.at("collider"), id);
        RigidBody2D body{collId};
        if (!phenyl_from_data(val, body)) {
            logging::log(LEVEL_WARNING, "Failed to parse serialised RigidBody2D!");
            destroyCollider(collId);
            return util::NullOpt;
        }

        return {body};
    });
    //serialiser.addComponentSerialiser<SimpleFriction>("SimpleFriction");
}

void PhysicsObject2D::updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) {
    for (auto [body, transform] : componentManager.iterate<RigidBody2D, common::GlobalTransform2D>()) {
        body.doMotion(transform, deltaTime);
    }
}

void PhysicsObject2D::checkCollisions (component::EntityComponentManager& compManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView, float deltaTime) {
    for (const auto& [body, transform] : compManager.iterate<RigidBody2D, common::GlobalTransform2D>()) {
        auto& collider = getCollider(body.getCollider());
        collider.updated = true;

        auto hitbox = ((Shape2D*)(shapeRegistry.getComponentErased(makePublicId(collider.hitbox))));
        hitbox->applyTransform(transform.transform2D.rotMatrix()); // TODO: dirty?
        hitbox->setPosition(transform.transform2D.position());

        collider.currentPos = transform.transform2D.position();
        collider.invMass = body.getInvMass();
        collider.invInertiaMoment = body.getInvInertia();
        collider.momentum = body.getMomentum();
        collider.angularMomentum = body.getAngularMomentum();
        //collider.elasticity = body.elasticity;

        collider.appliedImpulse = {0.0f, 0.0f};
        collider.appliedAngularImpulse = 0.0f;
    }

    // TODO: better way
    for (auto [id, coll] : colliders.iterate()) {
        if (coll.updated) {
            coll.updated = false;
        } else {
            logging::log(LEVEL_DEBUG, "Removing coll={}, shape={}", id + 1, coll.hitbox.getValue());
            shapeRegistry.remove(makePublicId(coll.hitbox));
            colliders.remove(id); // Valid because FLVector, so removal doesnt change other elements
        }
    }

    std::vector<std::tuple<ColliderId, ColliderId, Manifold2D>> collisionResults;
    auto iterable = shapeRegistry.iterate<BoxShape2D>();
    for (auto it1 = iterable.begin(); it1 != iterable.end(); ++it1) {
        auto [box1, id1] = *it1;
        auto it2 = it1;
        ++it2;
        for ( ; it2 != iterable.end(); ++it2) {
            auto [box2, id2] = *it2;
            if (!box1.shouldCollide(box2)) {
                continue;
            }
            box1.collide(box2)
                .ifPresent([&collisionResults, &box1, &box2] (SATResult2D result) {
                    auto face1 = box1.getSignificantFace(result.normal);
                    auto face2 = box2.getSignificantFace(-result.normal);

                    auto manifold = buildManifold(face1, face2, result.normal, result.depth);
                    collisionResults.emplace_back(box1.getColliderId(), box2.getColliderId(), manifold);
                });
        }
    }

    std::vector<std::tuple<component::EntityId, component::EntityId, std::uint32_t>> events;
    std::vector<Constraint2D> constraints;
    for (const auto& [id1, id2, manifold] : collisionResults) {
        manifold.buildConstraints(constraints, &getCollider(id1), &getCollider(id2), deltaTime);

        if (getCollider(id1).hitboxLayers & getCollider(id2).eventboxMask) {
            events.emplace_back(getCollider(id2).entityId, getCollider(id1).entityId, getCollider(id1).hitboxLayers & getCollider(id2).eventboxMask);
        }

        if (getCollider(id2).hitboxLayers & getCollider(id1).eventboxMask) {
            events.emplace_back(getCollider(id1).entityId, getCollider(id2).entityId, getCollider(id2).hitboxLayers & getCollider(id1).eventboxMask);
        }
    }

    solveConstraints(constraints, compManager);

    for (const auto& [id1, id2, layers] : events) {
        eventBus->raise(event::EntityCollisionEvent{compManager.view(id1), compManager.view(id2), layers, compManager, eventBus, gameView});
    }
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

    for (auto [id, collider] : colliders.iterate()) {
        auto& [transform, body] = compManager.get<common::GlobalTransform2D, RigidBody2D>(collider.entityId).getUnsafe();

        body.applyImpulse(collider.appliedImpulse);
        //transform.transform2D.translate(appliedImpulse / body.mass * deltaTime);

        body.applyAngularImpulse(collider.appliedAngularImpulse);
        //transform.transform2D.rotateBy(appliedAngularImpulse / body.inertialMoment * deltaTime);
    }
}


ColliderId PhysicsObject2D::addCollider (component::EntityId entityId) {
    auto index = colliders.emplace(entityId);
    return makeId<ColliderId>(index);
}

bool PhysicsObject2D::colliderExists (ColliderId id) const {
    return colliders.present(getIdIndex(id));
}

Collider2D& PhysicsObject2D::getCollider (ColliderId id) {
    return colliders.at(getIdIndex(id));
}

const Collider2D& PhysicsObject2D::getCollider (ColliderId id) const {
    return colliders.at(getIdIndex(id));
}


void PhysicsObject2D::destroyCollider (physics::ColliderId id) {
    auto index = getIdIndex(id);

    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to destroy collider {} that does not exist!", index);
    } else {
        colliders.remove(index);
    }
}

ShapeId PhysicsObject2D::getColliderHitbox (physics::ColliderId id) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to get hitbox of collider {} that does not exist!", getIdIndex(id));
        return {};
    }

    return getCollider(id).hitbox;
}

void PhysicsObject2D::setShapeType (physics::ShapeId id, physics::PrimitiveShape shape) {
    if (shape == PrimitiveShape::CUSTOM) {
        // TODO reconsider?
        logging::log(LEVEL_WARNING, "Cannot set shape to custom through setShapeType()!");
        return;
    }
}

ShapeDataNew PhysicsObject2D::getShapeData (physics::ShapeId id) const {
    auto pubId = makePublicId(id);
    auto shapeType = shapeRegistry.getTypeIndex(pubId);
    auto shapeData = shapeRegistry.getComponentErased(pubId);
    return {shapeType, (void*)shapeData}; // TODO: const qualify
}

ShapeId PhysicsObject2D::makeNewHitbox (physics::ColliderId colliderId, std::size_t typeIndex, void* request) {
    if (!colliderExists(colliderId)) {
        logging::log(LEVEL_WARNING, "Attempted to make new hitbox for colliderId {} that does not exist!", getIdIndex(colliderId));
        return {};
    }

    auto& collider = getCollider(colliderId);
    if (collider.hitbox) {
        shapeRegistry.remove(makePublicId(collider.hitbox));
    }

    auto id = makeShapeFromRequest(shapeRegistry, colliderId, typeIndex, request);

    collider.hitbox = id;

    return id;
}

void PhysicsObject2D::addEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    // TODO: pass through deserialiser instead
    scope = eventBus->getScope();
    /*eventBus->subscribe<event::EntityCreationEvent>([this] (event::EntityCreationEvent& event) {
        event.entityView.get<CollisionComponent2D>().ifPresent([this, &event] (CollisionComponent2D& comp) {
             if (!comp.collider) {
                 return;
             }

             auto& coll = getCollider(comp.collider);
             coll.entityId = event.entityView.id();
         });
    }, scope);*/

    eventBus->subscribe<event::DebugRenderEvent>([this] (const event::DebugRenderEvent& event) {
        debugColliderRender = event.doRender;
    }, scope);
}

void PhysicsObject2D::debugRender (const component::EntityComponentManager& componentManager) {
    if (debugColliderRender) {
        // Debug render
        for (const auto& [body, transform]: componentManager.iterate<RigidBody2D, common::GlobalTransform2D>()) {
            auto collider = getCollider(body.getCollider());
            shapeRegistry.getComponent<BoxShape2D>(makePublicId(collider.hitbox)).ifPresent(
                    [&transform] (const BoxShape2D& shape) {
                        auto pos1 = shape.getTransform() * glm::vec2{-1, -1} + transform.transform2D.position();
                        auto pos2 = shape.getTransform() * glm::vec2{1, -1} + transform.transform2D.position();
                        auto pos3 = shape.getTransform() * glm::vec2{1, 1} + transform.transform2D.position();
                        auto pos4 = shape.getTransform() * glm::vec2{-1, 1} + transform.transform2D.position();
                        //common::debugWorldRect(pos1, pos2, pos3, pos4, {1, 0, 0, 0.5}, {0, 0, 1, 1});
                        common::debugWorldRectOutline(pos1, pos2, pos3, pos4, {0, 0, 1, 1});
                    });
        }
    }
}
