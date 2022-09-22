#include "physics/physics.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/collision_component.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"
#include "collisions.h"
#include "common/events/entity_collision.h"

using namespace physics;

namespace {
    void updatePhysicsInternal (SimpleFrictionMotion2D& mainComp, component::Position2D& posComp) {
        // TODO: refactor to somewhere else
        int isPosXVel = mainComp.velocity.x > 0;
        int isPosYVel = mainComp.velocity.y > 0;

        mainComp.velocity.x -= ((float)mainComp.velocity.x * mainComp.linFriction) + mainComp.constFriction * (isPosXVel * 2 - 1);
        mainComp.velocity.y -= ((float)mainComp.velocity.y * mainComp.linFriction) + mainComp.constFriction * (isPosYVel * 2 - 1);

        mainComp.velocity.x *= (mainComp.velocity.x > 0 && isPosXVel) || (mainComp.velocity.x < 0 && !isPosXVel);
        mainComp.velocity.y *= (mainComp.velocity.y > 0 && isPosYVel) || (mainComp.velocity.y < 0 && !isPosYVel);

        mainComp.velocity += mainComp.acceleration;

        posComp += mainComp.velocity;
    }

    class DefaultPhysics : public IPhysics {
        void addComponentSerialisers (component::EntitySerialiser& serialiser) override {
            serialiser.addComponentSerialiser<SimpleFrictionMotion2D>("SimpleFrictionMotion2D");
            serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D");
        }

        void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) override {
            for (const auto& i : componentManager->getConstrainedView<SimpleFrictionMotion2D, component::Position2D>()) {
                updatePhysicsInternal(i.get<SimpleFrictionMotion2D>(), i.get<component::Position2D>());
            }
        }

        void checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView) override {
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
    };
}

std::unique_ptr<IPhysics> physics::makeDefaultPhysics () {
    return std::make_unique<DefaultPhysics>();
}