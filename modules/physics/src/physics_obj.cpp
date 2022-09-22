#include "physics/physics.h"
#include "component/component_serialiser.h"
#include "physics/components/2D/collision_component.h"
#include "component/components/2D/position.h"
#include "component/components/2D/rotation.h"
#include "collisions.h"
#include "common/events/entity_collision.h"
#include "physics/components/2D/kinematic_motion.h"

using namespace physics;

namespace {
    class DefaultPhysics : public IPhysics {
        void addComponentSerialisers (component::EntitySerialiser& serialiser) override {
            //serialiser.addComponentSerialiser<SimpleFrictionMotion2D>("SimpleFrictionMotion2D");
            serialiser.addComponentSerialiser<CollisionComponent2D>("CollisionComponent2D");
            serialiser.addComponentSerialiser<KinematicMotion2D>("KinematicMotion2D");
            serialiser.addComponentSerialiser<SimpleFriction>("SimpleFriction");
        }

        void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) override {
            for (const auto& i : componentManager->getConstrainedView<KinematicMotion2D, SimpleFriction>()) {
                i.get<SimpleFriction>().updateFriction2D(i.get<KinematicMotion2D>());
            }

            for (const auto& i : componentManager->getConstrainedView<KinematicMotion2D, component::Position2D>()) {
                //updatePhysicsInternal(i.get<SimpleFrictionMotion2D>(), i.get<component::Position2D>());
                i.get<KinematicMotion2D>().doMotion(i.get<component::Position2D>());
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
    };
}

std::unique_ptr<IPhysics> physics::makeDefaultPhysics () {
    return std::make_unique<DefaultPhysics>();
}