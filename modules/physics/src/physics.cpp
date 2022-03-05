#include "physics/physics.h"
#include "math.h"
#include "collisions.h"
#include "event/events/entity_collision.h"
#include "component/rotation_update.h"
#include <tuple>
#include "graphics/graphics_new_include.h"

using namespace physics;

/*void updatePhysicsInternal (component::EntityMainComponent* comp, int numEntities, int direction, std::pair<physics::CollisionComponent*,
                            graphics::AbsolutePosition*> comp2) {
    // TODO: consider writing fully in assembly
    for (int i = 0; i < numEntities; i ++) {
        int isPosXVel = comp->vel.x > 0;
        int isPosYVel = comp->vel.y > 0;
        //logging::logf(LEVEL_DEBUG, "Current pos before: (%f, %f). Current vel: (%f, %f).", comp->pos[0], comp->pos[1], comp->vel[0], comp->vel[1]);
        comp->vel.x -= ((float)comp->vel.x * comp->linFriction) + comp->constFriction * (isPosXVel * 2 - 1);
        comp->vel.y -= ((float)comp->vel.y * comp->linFriction) + comp->constFriction * (isPosYVel * 2 - 1);

        comp->vel.x *= (comp->vel.x > 0 && isPosXVel) || (comp->vel.x < 0 && !isPosXVel);
        comp->vel.y *= (comp->vel.y > 0 && isPosYVel) || (comp->vel.y < 0 && !isPosYVel);

        comp->vel += comp->acc;

        comp->pos += comp->vel;

        comp2.first->pos = comp->pos;
        comp2.second->pos = comp->pos;
        comp++;
        comp2.first++;
        comp2.second++;
        //logging::logf(LEVEL_DEBUG, "Current pos: (%f, %f). Current vel: (%f, %f).", comp->pos[0], comp->pos[1], comp->vel[0], comp->vel[1]);
    }
}*/

void updatePhysicsInternal (component::EntityMainComponent& mainComp, CollisionComponent& collComp, graphics::AbsolutePosition& posComp) {
    int isPosXVel = mainComp.vel.x > 0;
    int isPosYVel = mainComp.vel.y > 0;

    mainComp.vel.x -= ((float)mainComp.vel.x * mainComp.linFriction) + mainComp.constFriction * (isPosXVel * 2 - 1);
    mainComp.vel.y -= ((float)mainComp.vel.y * mainComp.linFriction) + mainComp.constFriction * (isPosYVel * 2 - 1);

    mainComp.vel.x *= (mainComp.vel.x > 0 && isPosXVel) || (mainComp.vel.x < 0 && !isPosXVel);
    mainComp.vel.y *= (mainComp.vel.y > 0 && isPosYVel) || (mainComp.vel.y < 0 && !isPosYVel);

    mainComp.vel += mainComp.acc;

    mainComp.pos += mainComp.vel;

    collComp.pos = mainComp.pos;
    posComp.pos = mainComp.pos;
}
void physics::onEntityCreation (event::EntityCreationEvent& event) {
    /*logging::log(LEVEL_DEBUG, "About to get main component!");
    auto comp = event.compManager->getObjectDataPtr<component::EntityMainComponent>(event.entityId);
    comp->pos[0] = event.x;
    comp->pos[1] = event.y;
    comp->vel[0] = 0;
    comp->vel[1] = 0;
    comp->acc[0] = 0;
    comp->acc[1] = 0;
    comp->linFriction = 0.27;
    comp->constFriction = 0.005; // TODO: remove hardcoding and add EntityType etc.
    auto comp2 = event.compManager->getObjectDataPtr<CollisionComponent>(event.entityId);
    auto comp3 = event.compManager->getObjectDataPtr<graphics::AbsolutePosition>(event.entityId);
    auto comp4 = event.compManager->getObjectDataPtr<component::RotationComponent>(event.entityId);
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
    /*componentManager->applyFunc<component::EntityMainComponent>(updatePhysicsInternal, std::pair(componentManager->getComponent<CollisionComponent>().orElse(nullptr),
                                        componentManager->getComponent<graphics::AbsolutePosition>().orElse(nullptr)));*/
    /*for (auto i : *componentManager) {
        i.applyFunc<component::EntityMainComponent, CollisionComponent, graphics::AbsolutePosition>(updatePhysicsInternal);
    }*/
    for (const auto& i : componentManager->getConstrainedView<component::EntityMainComponent, CollisionComponent, graphics::AbsolutePosition>()) {
        updatePhysicsInternal(i.get<component::EntityMainComponent>(), i.get<CollisionComponent>(), i.get<graphics::AbsolutePosition>());
    }
}

void physics::checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView gameView) {
    //logging::log(LEVEL_DEBUG, "Checking collisions!");
    std::vector<std::tuple<component::EntityId, component::EntityId, glm::vec2>> collisionResults; // TODO: do caching or something
    collisionResults.reserve(componentManager->getNumObjects());

    /*componentManager->applyFunc<CollisionComponent, component::RotationComponent>([](CollisionComponent* coll, component::RotationComponent* rot, int numEntities, int direction) {
        for (int i = 0; i < numEntities; i++) {
            coll[i].bbMap *= rot[i].rotMatrix;
        }
    });*/

    /*for (auto i : *componentManager) {
        i.applyFunc<CollisionComponent, component::RotationComponent>([](CollisionComponent& coll, component::RotationComponent& rot) {
            coll.bbMap *= rot.rotMatrix;
        });
    }*/
    for (const auto& i : componentManager->getConstrainedView<CollisionComponent, component::RotationComponent>()) {
        i.get<CollisionComponent>().bbMap *= i.get<component::RotationComponent>().rotMatrix;
    }

    //componentManager->applyFunc<CollisionComponent, component::EntityId>(checkCollisionsEntity, &collisionResults);
    checkCollisionsEntity(componentManager, collisionResults);

    /*componentManager->applyFunc<CollisionComponent, component::RotationComponent>([](CollisionComponent* coll, component::RotationComponent* rot, int numEntities, int direction) {
        for (int i = 0; i < numEntities; i++) {
            coll[i].bbMap *= glm::inverse(rot[i].rotMatrix);
        }
    });*/
    /*for (auto i : *componentManager) {
        i.applyFunc<CollisionComponent, component::RotationComponent>([](CollisionComponent& coll, component::RotationComponent& rot) {
            coll.bbMap *= glm::inverse(rot.rotMatrix);
        });
    }*/
    for (const auto& i : componentManager->getConstrainedView<CollisionComponent, component::RotationComponent>()) {
        i.get<CollisionComponent>().bbMap *= glm::inverse(i.get<component::RotationComponent>().rotMatrix);
    }

    for (auto p : collisionResults) {
        auto [x,y,dVec] = p;
        logging::log(LEVEL_DEBUG, "Detected collision between entities {} and {} with min translation vec <{}, {}>!", x.value(), y.value(), dVec.x, dVec.y);
        // TODO: collision event
        auto comp1Opt = componentManager->getObjectData<CollisionComponent>(x);
        auto comp2Opt = componentManager->getObjectData<CollisionComponent>(y);

        auto& dVec1 = dVec;
        auto& x1 = x;
        auto& y1 = y;
        comp1Opt.ifPresent([&comp2Opt, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView](auto& comp1) {
            comp2Opt.ifPresent([&comp1, &componentManager, &dVec1, &x1, &y1, &eventBus, &gameView](auto& comp2) {
                auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
                auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
                float totalMass = comp1Mass + comp2Mass;
                if (totalMass != 0) {
                    componentManager->getObjectDataPtr<component::EntityMainComponent>(x1).orElse(nullptr)->pos +=
                            dVec1 * comp1Mass / totalMass;
                    componentManager->getObjectDataPtr<component::EntityMainComponent>(x1).orElse(nullptr)->vel -=
                            projectVec(dVec1, componentManager->getObjectData<component::EntityMainComponent>(x1).orElse(component::EntityMainComponent()).vel);
                    componentManager->getObjectDataPtr<component::EntityMainComponent>(y1).orElse(nullptr)->pos -=
                            dVec1 * comp2Mass / totalMass;
                    componentManager->getObjectDataPtr<component::EntityMainComponent>(y1).orElse(nullptr)->vel -=
                            projectVec(dVec1, componentManager->getObjectData<component::EntityMainComponent>(y1).orElse(component::EntityMainComponent()).vel);
                }
                if (comp1.layers & comp2.eventLayer) {
                    eventBus->raiseEvent(event::EntityCollisionEvent(y1, x1, comp1.layers & comp2.eventLayer, componentManager, eventBus, gameView));
                }
                if (comp2.layers & comp1.eventLayer) {
                    eventBus->raiseEvent(event::EntityCollisionEvent(x1, y1, comp2.layers & comp1.eventLayer, componentManager, eventBus, gameView));
                }
            });
        });

        /*auto comp2 = componentManager->getObjectData<CollisionComponent>(y);
        auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
        auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
        float totalMass = comp1Mass + comp2Mass;
        if (totalMass != 0) {
            componentManager->getObjectDataPtr<component::EntityMainComponent>(x)->pos +=
                    dVec * comp1Mass / totalMass;
            componentManager->getObjectDataPtr<component::EntityMainComponent>(x)->vel -=
                    projectVec(dVec, componentManager->getObjectData<component::EntityMainComponent>(x).vel);
            componentManager->getObjectDataPtr<component::EntityMainComponent>(y)->pos -=
                    dVec * comp2Mass / totalMass;
            componentManager->getObjectDataPtr<component::EntityMainComponent>(y)->vel -=
                    projectVec(dVec, componentManager->getObjectData<component::EntityMainComponent>(y).vel);
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
    auto ptr = event.manager->getObjectDataPtr<CollisionComponent>(event.entityId);
    event.manager->getObjectData<CollisionComponent>(event.entityId).ifPresent([&event](auto& ptr) {
        ptr.bbMap *= event.rotMatrix;
    });
    //ptr->bbMap *= event.rotMatrix;
}