#include "physics/physics.h"
#include "math.h"
#include "collisions.h"
#include "common/events/entity_collision.h"
#include <tuple>
#include "graphics/graphics_new_include.h"
#include "component/position.h"

using namespace physics;

/*void updatePhysicsInternal (component::FrictionKinematicsMotion2D* comp, int numEntities, int direction, std::pair<physics::CollisionComponent2D*,
                            graphics::Transform2D*> comp2) {
    // TODO: consider writing fully in assembly
    for (int i = 0; i < numEntities; i ++) {
        int isPosXVel = comp->velocity.x > 0;
        int isPosYVel = comp->velocity.y > 0;
        //logging::logf(LEVEL_DEBUG, "Current pos before: (%f, %f). Current velocity: (%f, %f).", comp->pos[0], comp->pos[1], comp->velocity[0], comp->velocity[1]);
        comp->velocity.x -= ((float)comp->velocity.x * comp->linFriction) + comp->constFriction * (isPosXVel * 2 - 1);
        comp->velocity.y -= ((float)comp->velocity.y * comp->linFriction) + comp->constFriction * (isPosYVel * 2 - 1);

        comp->velocity.x *= (comp->velocity.x > 0 && isPosXVel) || (comp->velocity.x < 0 && !isPosXVel);
        comp->velocity.y *= (comp->velocity.y > 0 && isPosYVel) || (comp->velocity.y < 0 && !isPosYVel);

        comp->velocity += comp->acceleration;

        comp->pos += comp->velocity;

        comp2.first->pos = comp->pos;
        comp2.second->pos = comp->pos;
        comp++;
        comp2.first++;
        comp2.second++;
        //logging::logf(LEVEL_DEBUG, "Current pos: (%f, %f). Current velocity: (%f, %f).", comp->pos[0], comp->pos[1], comp->velocity[0], comp->velocity[1]);
    }
}*/

void updatePhysicsInternal (component::FrictionKinematicsMotion2D& mainComp, component::Position2D& posComp) {
    int isPosXVel = mainComp.velocity.x > 0;
    int isPosYVel = mainComp.velocity.y > 0;

    mainComp.velocity.x -= ((float)mainComp.velocity.x * mainComp.linFriction) + mainComp.constFriction * (isPosXVel * 2 - 1);
    mainComp.velocity.y -= ((float)mainComp.velocity.y * mainComp.linFriction) + mainComp.constFriction * (isPosYVel * 2 - 1);

    mainComp.velocity.x *= (mainComp.velocity.x > 0 && isPosXVel) || (mainComp.velocity.x < 0 && !isPosXVel);
    mainComp.velocity.y *= (mainComp.velocity.y > 0 && isPosYVel) || (mainComp.velocity.y < 0 && !isPosYVel);

    mainComp.velocity += mainComp.acceleration;

    posComp += mainComp.velocity;
}
void physics::onEntityCreation (event::EntityCreationEvent& event) {
    /*logging::log(LEVEL_DEBUG, "About to get main component!");
    auto comp = event.compManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(event.entityId);
    comp->pos[0] = event.x;
    comp->pos[1] = event.y;
    comp->velocity[0] = 0;
    comp->velocity[1] = 0;
    comp->acceleration[0] = 0;
    comp->acceleration[1] = 0;
    comp->linFriction = 0.27;
    comp->constFriction = 0.005; // TODO: remove hardcoding and add EntityType etc.
    auto comp2 = event.compManager->getObjectDataPtr<CollisionComponent2D>(event.entityId);
    auto comp3 = event.compManager->getObjectDataPtr<graphics::Transform2D>(event.entityId);
    auto comp4 = event.compManager->getObjectDataPtr<component::Rotation2D>(event.entityId);
    comp2->pos = comp->pos;
    comp2->bbMap = {{event.size / 2, 0.0f}, {0.0f, event.size / 2}};
    comp2->masks = 1; // TODO: move to EntityType
    comp2->layers = 1;
    comp2->eventLayer = 1;
    comp2->mass = 10.0f;
    comp2->outerRadius = sqrt(2) * event.size;
    comp3->pos = comp->pos;
    comp3->transform = comp2->bbMap;
    comp4->rotation = 0;
    comp4->rotMatrix = {{1, 0}, {0, 1}};
    if (event.entityId == 0) {
        // TODO: remove
        comp2->resolveLayers = 1;
        //component::rotateEntity(event.entityId, M_PI / 4, event.compManager, event.eventBus);
    }*/
}

void physics::updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) {
    /*componentManager->applyFunc<component::FrictionKinematicsMotion2D>(updatePhysicsInternal, std::pair(componentManager->getComponent<CollisionComponent2D>().orElse(nullptr),
                                        componentManager->getComponent<graphics::Transform2D>().orElse(nullptr)));*/
    /*for (auto i : *componentManager) {
        i.applyFunc<component::FrictionKinematicsMotion2D, CollisionComponent2D, graphics::Transform2D>(updatePhysicsInternal);
    }*/
    for (const auto& i : componentManager->getConstrainedView<component::FrictionKinematicsMotion2D, component::Position2D>()) {
        updatePhysicsInternal(i.get<component::FrictionKinematicsMotion2D>(), i.get<component::Position2D>());
    }
}

void physics::checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView gameView) {
    //logging::log(LEVEL_DEBUG, "Checking collisions!");
    std::vector<std::tuple<component::EntityId, component::EntityId, glm::vec2>> collisionResults; // TODO: do caching or something
    collisionResults.reserve(componentManager->getNumObjects());

    /*componentManager->applyFunc<CollisionComponent2D, component::Rotation2D>([](CollisionComponent2D* coll, component::Rotation2D* rot, int numEntities, int direction) {
        for (int i = 0; i < numEntities; i++) {
            coll[i].bbMap *= rot[i].rotMatrix;
        }
    });*/

    /*for (auto i : *componentManager) {
        i.applyFunc<CollisionComponent2D, component::Rotation2D>([](CollisionComponent2D& coll, component::Rotation2D& rot) {
            coll.bbMap *= rot.rotMatrix;
        });
    }*/
    for (const auto& i : componentManager->getConstrainedView<CollisionComponent2D, component::Rotation2D>()) {
        i.get<CollisionComponent2D>().rotBBMap = i.get<CollisionComponent2D>().bbMap * i.get<component::Rotation2D>().rotMatrix;
    }

    //componentManager->applyFunc<CollisionComponent2D, component::EntityId>(checkCollisionsEntity, &collisionResults);
    checkCollisionsEntity(componentManager, collisionResults);

    /*componentManager->applyFunc<CollisionComponent2D, component::Rotation2D>([](CollisionComponent2D* coll, component::Rotation2D* rot, int numEntities, int direction) {
        for (int i = 0; i < numEntities; i++) {
            coll[i].bbMap *= glm::inverse(rot[i].rotMatrix);
        }
    });*/
    /*for (auto i : *componentManager) {
        i.applyFunc<CollisionComponent2D, component::Rotation2D>([](CollisionComponent2D& coll, component::Rotation2D& rot) {
            coll.bbMap *= glm::inverse(rot.rotMatrix);
        });
    }*/
    /*for (const auto& i : componentManager->getConstrainedView<CollisionComponent2D, component::Rotation2D>()) {
        i.get<CollisionComponent2D>().bbMap *= glm::inverse(i.get<component::Rotation2D>().rotMatrix);
    }*/

    for (auto p : collisionResults) {
        auto [x,y,dVec] = p;
        logging::log(LEVEL_DEBUG, "Detected collision between entities {} and {} with min translation vec <{}, {}>!", x.value(), y.value(), dVec.x, dVec.y);
        // TODO: collision event
        auto comp1Opt = componentManager->getObjectData<CollisionComponent2D>(x);
        auto comp2Opt = componentManager->getObjectData<CollisionComponent2D>(y);

        auto& dVec1 = dVec;
        auto& x1 = x;
        auto& y1 = y;
        comp1Opt.ifPresent([&comp2Opt, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView](auto& comp1) {
            comp2Opt.ifPresent([&comp1, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView](auto& comp2) {
                auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
                auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
                float totalMass = comp1Mass + comp2Mass;
                if (totalMass != 0) {
                    *componentManager->getObjectDataPtr<component::Position2D>(x1).orElse(nullptr) +=
                            dVec1 * comp1Mass / totalMass;
                    componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(x1).orElse(nullptr)->velocity -=
                            projectVec(dVec1, componentManager->getObjectData<component::FrictionKinematicsMotion2D>(x1).orElse(component::FrictionKinematicsMotion2D()).velocity);
                    *componentManager->getObjectDataPtr<component::Position2D>(y1).orElse(nullptr) -=
                            dVec1 * comp2Mass / totalMass;
                    componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(y1).orElse(nullptr)->velocity -=
                            projectVec(dVec1, componentManager->getObjectData<component::FrictionKinematicsMotion2D>(y1).orElse(component::FrictionKinematicsMotion2D()).velocity);
                }
                if (comp1.layers & comp2.eventLayer) {
                    eventBus->raise(
                            event::EntityCollisionEvent(y1, x1, comp1.layers & comp2.eventLayer, componentManager,
                                                        eventBus, gameView));
                }
                if (comp2.layers & comp1.eventLayer) {
                    eventBus->raise(
                            event::EntityCollisionEvent(x1, y1, comp2.layers & comp1.eventLayer, componentManager,
                                                        eventBus, gameView));
                }
            });
        });

        /*auto comp2 = componentManager->getObjectData<CollisionComponent2D>(y);
        auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
        auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
        float totalMass = comp1Mass + comp2Mass;
        if (totalMass != 0) {
            componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(x)->pos +=
                    dVec * comp1Mass / totalMass;
            componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(x)->velocity -=
                    projectVec(dVec, componentManager->getObjectData<component::FrictionKinematicsMotion2D>(x).velocity);
            componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(y)->pos -=
                    dVec * comp2Mass / totalMass;
            componentManager->getObjectDataPtr<component::FrictionKinematicsMotion2D>(y)->velocity -=
                    projectVec(dVec, componentManager->getObjectData<component::FrictionKinematicsMotion2D>(y).velocity);
        }
        if (comp1.layers & comp2.eventLayer) {
            eventBus->raiseEvent(event::EntityCollisionEvent(y, x, comp1.layers & comp2.eventLayer, componentManager, eventBus, gameView));
        }
        if (comp2.layers & comp1.eventLayer) {
            eventBus->raiseEvent(event::EntityCollisionEvent(x, y, comp2.layers & comp1.eventLayer, componentManager, eventBus, gameView));
        }*/
    }
}
void physics::updateEntityHitboxRotation (event::EntityRotationEvent& event) {
    auto ptr = event.manager->getObjectDataPtr<CollisionComponent2D>(event.entityId);
    event.manager->getObjectData<CollisionComponent2D>(event.entityId).ifPresent([&event](auto& ptr) {
        ptr.bbMap *= event.rotMatrix;
    });
    //ptr->bbMap *= event.rotMatrix;
}