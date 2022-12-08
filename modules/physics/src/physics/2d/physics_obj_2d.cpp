#include "physics_obj_2d.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/collision_component.h"
#include "physics/components/2D/kinematic_motion.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"
#include "physics/collisions.h"
#include "common/events/entity_collision.h"

using namespace physics;

void PhysicsObject2D::addComponentSerialisers (component::EntitySerialiser& serialiser) {
    //serialiser.addComponentSerialiser<SimpleFrictionMotion2D>("SimpleFrictionMotion2D");
    serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D");
    serialiser.addComponentSerialiser<KinematicMotion2D>("KinematicMotion2D");
    serialiser.addComponentSerialiser<SimpleFriction>("SimpleFriction");
}

void PhysicsObject2D::updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) {
    for (const auto& i : componentManager->getConstrainedView<KinematicMotion2D, SimpleFriction>()) {
        i.get<SimpleFriction>().updateFriction2D(i.get<KinematicMotion2D>());
    }

    for (const auto& i : componentManager->getConstrainedView<KinematicMotion2D, component::Position2D>()) {
        //updatePhysicsInternal(i.get<SimpleFrictionMotion2D>(), i.get<component::Position2D>());
        i.get<KinematicMotion2D>().doMotion(i.get<component::Position2D>());
    }
}

void PhysicsObject2D::checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus,
                                       view::GameView& gameView) {
    // TODO: refactor

    std::vector<std::tuple<component::EntityId, component::EntityId, glm::vec2>> collisionResults; // TODO: do caching or something
    collisionResults.reserve(componentManager->getNumObjects());

    for (const auto& i: componentManager->getConstrainedView<CollisionComponent2D, component::Rotation2D>()) {
        i.get<CollisionComponent2D>().rotBBMap =
                i.get<CollisionComponent2D>().bbMap * i.get<component::Rotation2D>().rotMatrix;
    }

    checkCollisionsEntity(componentManager, collisionResults);

    for (auto& p: collisionResults) {
        auto& [x, y, dVec] = p;
        logging::log(LEVEL_DEBUG, "Detected collision between entities {} and {} with min translation vec <{}, {}>!",
                     x.value(), y.value(), dVec.x, dVec.y);
        // TODO: collision event
        auto comp1Opt = componentManager->getObjectData<CollisionComponent2D>(x);
        auto comp2Opt = componentManager->getObjectData<CollisionComponent2D>(y);

        auto& dVec1 = dVec;
        auto& x1 = x;
        auto& y1 = y;
        comp1Opt.ifPresent([&comp2Opt, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView] (auto& comp1) {
            comp2Opt.ifPresent([&comp1, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView] (auto& comp2) {
                auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
                auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
                float totalMass = comp1Mass + comp2Mass;
                if (totalMass != 0) {
                    componentManager->getObjectData<component::Position2D>(x1).ifPresent([&dVec1, &comp1Mass, &totalMass](component::Position2D& pos2D) {
                        pos2D += dVec1 * comp1Mass / totalMass;
                    });

                    componentManager->getObjectData<KinematicMotion2D>(x1).ifPresent([&dVec1] (KinematicMotion2D& comp) {
                        comp.velocity -= projectVec(dVec1, comp.velocity);
                    });

                    componentManager->getObjectData<component::Position2D>(y1).ifPresent([&dVec1, &comp2Mass, &totalMass](component::Position2D& pos2D) {
                        pos2D += dVec1 * comp2Mass / totalMass;
                    });

                    componentManager->getObjectData<KinematicMotion2D>(y1).ifPresent([&dVec1] (KinematicMotion2D& comp) {
                        comp.velocity -= projectVec(-dVec1, comp.velocity);
                    });
                }
                if (comp1.layers & comp2.eventLayer) {
                    eventBus->raise(
                            event::EntityCollisionEvent(componentManager->getEntityView(y1),
                                                        componentManager->getEntityView(x1),
                                                        comp1.layers & comp2.eventLayer, componentManager,
                                                        eventBus, gameView));
                }
                if (comp2.layers & comp1.eventLayer) {
                    eventBus->raise(
                            event::EntityCollisionEvent(componentManager->getEntityView(x1),
                                                        componentManager->getEntityView(y1),
                                                        comp2.layers & comp1.eventLayer, componentManager,
                                                        eventBus, gameView));
                }
            });
        });
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

ShapeId PhysicsObject2D::getColliderEventbox (physics::ColliderId id) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to get eventbox of collider {} that does not exist!", getIdIndex(id));
        return {};
    }
    return getCollider(id).eventbox;
}

bool PhysicsObject2D::colliderShapesMerged (physics::ColliderId id) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to get shapes merged of collider {} that does not exist!", getIdIndex(id));
        return false;
    }
    return getCollider(id).shapesMerged;
}

void PhysicsObject2D::setColliderShapesMerged (physics::ColliderId id, bool merged) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to set shapes merged of collider {} that does not exist!", getIdIndex(id));
    }

    getCollider(id).shapesMerged = merged;
}


void PhysicsObject2D::setShapeType (physics::ShapeId id, physics::PrimitiveShape shape) {
    if (shape == PrimitiveShape::CUSTOM) {
        // TODO reconsider?
        logging::log(LEVEL_WARNING, "Cannot set shape to custom through setShapeType()!");
        return;
    }
}

ShapeData PhysicsObject2D::getShapeData (physics::ShapeId id) {
    return {0, nullptr};
}

