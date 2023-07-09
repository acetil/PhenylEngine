#include "physics_obj_2d.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/collision_component.h"
#include "physics/components/2D/kinematic_motion.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"
#include "common/events/entity_collision.h"
#include "common/events/entity_creation.h"

#include "physics/shape/2d/box_shape_2d_interface.h"

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

void PhysicsObject2D::addComponentSerialisers (component::EntitySerialiser& serialiser) {
    //serialiser.addComponentSerialiser<SimpleFrictionMotion2D>("SimpleFrictionMotion2D");
    //serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D");
    serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D", [this] (const CollisionComponent2D& comp) -> util::DataValue {
        util::DataObject obj;
        obj["coll_scale"] = comp.transform;

        const auto& coll = getCollider(comp.collider);
        obj["mass"] = coll.mass;
        obj["layers"] = static_cast<unsigned int>(coll.hitboxLayers); // TODO: remove static cast
        obj["masks"] = static_cast<unsigned int>(coll.eventboxMask);
        obj["resolve_layers"] = static_cast<unsigned int>(coll.eventboxMask);
        obj["event_layers"] = static_cast<unsigned int>(coll.eventboxMask);
        obj["shape"] = ((Shape2D*)shapeRegistry.getComponentErased(makePublicId(coll.hitbox)))->serialise();

        return obj;
    }, [this] (const util::DataValue& val) -> util::Optional<CollisionComponent2D> {
        if (!val.is<util::DataObject>()) {
            return util::NullOpt;
        }
        const auto& obj = val.get<util::DataObject>();


        CollisionComponent2D comp;
        if (!obj.contains("coll_scale") || !phenyl_from_data(obj.at("coll_scale"), comp.transform)) {
            return util::NullOpt;
        }

        Collider2D coll;
        if (!obj.contains("mass") || !obj.at("mass").is<float>()) {
            return util::NullOpt;
        }
        coll.mass = obj.at("mass").get<float>();

        if (!obj.contains("layers") || !obj.at("layers").is<int>()) { // TODO
            return util::NullOpt;
        }
        coll.hitboxLayers = (std::size_t)obj.at("layers").get<unsigned int>();

        if (!obj.contains("masks") || !obj.at("masks").is<int>()) { // TODO
            return util::NullOpt;
        }
        coll.eventboxMask = (std::size_t)obj.at("masks").get<unsigned int>();

        if (!obj.contains("shape")) {
            return util::NullOpt;
        }
        auto collId = makeId<ColliderId>(colliders.push(coll));
        auto shapeId = deserialiseShape(obj.at("shape"), collId, coll.hitboxLayers, coll.eventboxMask);
        if (!shapeId) {
            destroyCollider(collId);
            return util::NullOpt;
        }
        getCollider(collId).hitbox = shapeId;
        comp.collider = collId;
        return {comp};
    });
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

void PhysicsObject2D::resolveCollision (physics::ColliderId id1, physics::ColliderId id2, glm::vec2 disp, const component::EntityComponentManager::SharedPtr& compManager) {
    const auto& coll1 = getCollider(id1);
    const auto& coll2 = getCollider(id2);

    auto totalMass = coll1.mass + coll2.mass;
    if (totalMass > 0) {
        auto res1 = (-disp) * coll1.mass / totalMass;
        logging::log(LEVEL_DEBUG, "Collision resolution for {}: <{}, {}>", coll1.entityId.value(), res1.x, res1.y);
        compManager->getObjectData<component::Position2D>(coll1.entityId).getUnsafe() += res1;
        auto res2 = disp * coll2.mass / totalMass;
        logging::log(LEVEL_DEBUG, "Collision resolution for {}: <{}, {}>", coll2.entityId.value(), res2.x, res2.y);
        compManager->getObjectData<component::Position2D>(coll2.entityId).getUnsafe() += res2;

        // TODO: evaluate if necessary
        /*compManager->getObjectData<KinematicMotion2D>(coll1.entityId).ifPresent([&disp, &coll1, &totalMass] (KinematicMotion2D& comp) {
            comp.velocity -= projectVec(disp * (coll1.mass / totalMass), comp.velocity);
        });

        compManager->getObjectData<KinematicMotion2D>(coll2.entityId).ifPresent([&disp, &coll2, &totalMass] (KinematicMotion2D& comp) {
            comp.velocity -= projectVec(-disp * (coll2.mass / totalMass), comp.velocity);
        });*/
    }
}

void PhysicsObject2D::checkCollisions (const component::EntityComponentManager::SharedPtr& compManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView) {
    std::vector<std::tuple<ColliderId, ColliderId, glm::vec2>> collisionResults;
    //logging::log(LEVEL_DEBUG, "===New collision check===");
    for (const auto& i : compManager->getConstrainedView<CollisionComponent2D, component::Rotation2D, component::Position2D>()) {
        //logging::log(LEVEL_DEBUG, "{}: collId={}", i.getId().value(), i.get<CollisionComponent2D>().collider.getValue());
        auto& collider = getCollider(i.get<CollisionComponent2D>().collider);
        //logging::log(LEVEL_DEBUG, "{}: shapeId={}", i.getId().value(), collider.hitbox.getValue());
        collider.updated = true;

        auto hitbox = ((Shape2D*)(shapeRegistry.getComponentErased(makePublicId(collider.hitbox))));
        hitbox->applyTransform(i.get<component::Rotation2D>().rotMatrix * i.get<CollisionComponent2D>().transform); // TODO: dirty?
        hitbox->setPosition(i.get<component::Position2D>().get());
    }

    for (auto [id, coll] : colliders.iterate()) {
        if (coll.updated) {
            coll.updated = false;
        } else {
            logging::log(LEVEL_DEBUG, "Removing coll={}, shape={}", id + 1, coll.hitbox.getValue());
            shapeRegistry.remove(makePublicId(coll.hitbox));
            colliders.remove(id); // Valid because FLVector, so removal doesnt change other elements
        }
    }

    /*for (const auto& [box1, id1] : shapeRegistry.iterate<BoxShape2D>()) {
        for (const auto& [box2, id2] : shapeRegistry.iterate<BoxShape2D>()) {
            if (id1 == id2 || !(box1.shouldCollide(box2))) {
                continue;
            }

            box1.collide(box2)
                .ifPresent([&collisionResults, &box1, &box2] (glm::vec2 vec) {
                    collisionResults.emplace_back(box1.getColliderId(), box2.getColliderId(), vec);
                });
        }
    }*/

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
                .ifPresent([&collisionResults, &box1, &box2] (glm::vec2 vec) {
                    collisionResults.emplace_back(box1.getColliderId(), box2.getColliderId(), vec);
                });
        }
    }

    std::vector<std::tuple<component::EntityId, component::EntityId, std::uint32_t>> events;
    for (const auto& [id1, id2, disp] : collisionResults) {
        logging::log(LEVEL_DEBUG, "Detected collision between entities {} and {} with min translation vec <{}, {}>!",
                     getCollider(id1).entityId.value(), getCollider(id2).entityId.value(), disp.x, disp.y);

        resolveCollision(id1, id2, disp, compManager);

        if (getCollider(id1).hitboxLayers & getCollider(id2).eventboxMask) {
            events.emplace_back(getCollider(id2).entityId, getCollider(id1).entityId, getCollider(id1).hitboxLayers & getCollider(id2).eventboxMask);
        }

        if (getCollider(id2).hitboxLayers & getCollider(id1).eventboxMask) {
            events.emplace_back(getCollider(id1).entityId, getCollider(id2).entityId, getCollider(id2).hitboxLayers & getCollider(id1).eventboxMask);
        }
    }

    for (const auto& [id1, id2, layers] : events) {
        eventBus->raise(event::EntityCollisionEvent{compManager->getEntityView(id1), compManager->getEntityView(id2), layers, compManager, eventBus, gameView});
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

/*ShapeId PhysicsObject2D::getColliderEventbox (physics::ColliderId id) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to get eventbox of collider {} that does not exist!", getIdIndex(id));
        return {};
    }
    return getCollider(id).hitbox;
}*/

/*bool PhysicsObject2D::colliderShapesMerged (physics::ColliderId id) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to get shapes merged of collider {} that does not exist!", getIdIndex(id));
        return false;
    }
    return getCollider(id).shapesMerged;
}*/

/*void PhysicsObject2D::setColliderShapesMerged (physics::ColliderId id, bool merged) {
    if (!colliderExists(id)) {
        logging::log(LEVEL_WARNING, "Attempted to set shapes merged of collider {} that does not exist!", getIdIndex(id));
    }

    getCollider(id).shapesMerged = merged;
}*/


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

/*ShapeId PhysicsObject2D::makeNewEventbox (physics::ColliderId colliderId, std::size_t typeIndex, void* request) {
    if (!colliderExists(colliderId)) {
        logging::log(LEVEL_WARNING, "Attempted to make new hitbox for colliderId {} that does not exist!", getIdIndex(colliderId));
        return {};
    }

    auto& collider = getCollider(colliderId);
    if (collider.eventbox) {
        shapeRegistry.remove(makePublicId(collider.hitbox));
    }

    auto id = makeShapeFromRequest(shapeRegistry, colliderId, typeIndex, request);

    collider.eventbox = id;

    return id;
    return {};
}*/

void PhysicsObject2D::addEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    // TODO: pass through deserialiser instead
    scope = eventBus->getScope();
    eventBus->subscribe<event::EntityCreationEvent>([this] (event::EntityCreationEvent& event) {
         event.entityView.getComponent<CollisionComponent2D>().ifPresent([this, &event] (CollisionComponent2D& comp) {
             if (!comp.collider) {
                 return;
             }

             auto& coll = getCollider(comp.collider);
             coll.entityId = event.entityView.getId();
         });
    }, scope);
}

