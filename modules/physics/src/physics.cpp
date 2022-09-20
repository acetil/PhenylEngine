#include "physics/physics.h"
#include "math.h"
#include "collisions.h"
#include "common/events/entity_collision.h"
#include <tuple>
#include "graphics/graphics_new_include.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/simple_friction.h"

using namespace physics;

/*void updatePhysicsInternal (component::SimpleFrictionMotion2D* comp, int numEntities, int direction, std::pair<physics::CollisionComponent2D*,
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

void updatePhysicsInternal (SimpleFrictionMotion2D& mainComp, component::Position2D& posComp) {
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
}

void physics::updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) {
    for (const auto& i : componentManager->getConstrainedView<SimpleFrictionMotion2D, component::Position2D>()) {
        updatePhysicsInternal(i.get<SimpleFrictionMotion2D>(), i.get<component::Position2D>());
    }
}

void physics::checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView gameView) {
    //logging::log(LEVEL_DEBUG, "Checking collisions!");
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
                    *componentManager->getObjectDataPtr<component::Position2D>(x1).orElse(nullptr) +=
                            dVec1 * comp1Mass / totalMass;
                    componentManager->getObjectDataPtr<SimpleFrictionMotion2D>(x1).orElse(
                            nullptr)->velocity -=
                            projectVec(dVec1, componentManager->getObjectData<SimpleFrictionMotion2D>(
                                    x1).orElse(SimpleFrictionMotion2D()).velocity);
                    *componentManager->getObjectDataPtr<component::Position2D>(y1).orElse(nullptr) -=
                            dVec1 * comp2Mass / totalMass;
                    componentManager->getObjectDataPtr<SimpleFrictionMotion2D>(y1).orElse(
                            nullptr)->velocity -=
                            projectVec(dVec1, componentManager->getObjectData<SimpleFrictionMotion2D>(
                                    y1).orElse(SimpleFrictionMotion2D()).velocity);
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

void physics::addComponentSerialisers (component::EntitySerialiser& serialiser) {
    serialiser.addComponentSerialiser<SimpleFrictionMotion2D>("SimpleFrictionMotion2D");
    serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D");
}