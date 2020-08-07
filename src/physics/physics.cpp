#include "physics.h"
#include "math.h"
#include "collisions.h"
#include "event/events/entity_collision.h"
#include <tuple>
#include <graphics/graphics_new_include.h>

using namespace physics;

void updatePhysicsInternal (component::EntityMainComponent* comp, int numEntities, int direction, std::pair<physics::CollisionComponent*,
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
        comp += direction;
        comp2.first += direction;
        comp2.second += direction;
        //logging::logf(LEVEL_DEBUG, "Current pos: (%f, %f). Current vel: (%f, %f).", comp->pos[0], comp->pos[1], comp->vel[0], comp->vel[1]);
    }
}
void physics::onEntityCreation (event::EntityCreationEvent& event) {
    logging::log(LEVEL_DEBUG, "About to get main component!");
    auto* comp = event.compManager->getObjectDataPtr<component::EntityMainComponent>(1, event.entityId);
    comp->pos[0] = event.x;
    comp->pos[1] = event.y;
    comp->vel[0] = 0;
    comp->vel[1] = 0;
    comp->acc[0] = 0;
    comp->acc[1] = 0;
    comp->linFriction = 0.2;
    comp->constFriction = 0.002; // TODO: remove hardcoding and add EntityType etc.
    auto comp2 = event.compManager->getObjectDataPtr<CollisionComponent>(3, event.entityId);
    auto comp3 = event.compManager->getObjectDataPtr<graphics::AbsolutePosition>(4, event.entityId);
    comp2->pos = comp->pos;
    comp2->bbMap = {{event.size / 2, 0.0f}, {0.0f, event.size / 2}};
    comp2->masks = 1; // TODO: move to EntityType
    comp2->layers = 1;
    comp2->mass = 10.0f;
    if (event.entityId == 0) {
        // TODO: remove
        float val = 1 /sqrt(2);
        //printf("%f\n", val);
        comp2->bbMap *= glm::mat2{{val,-val}, {val, val}}; // pi/4 rotation
        comp2->resolveLayers = 1;
        comp2->eventLayer = 1;
    }
    comp2->outerRadius = sqrt(2) * event.size;
    comp3->pos = comp->pos;
    comp3->transform = comp2->bbMap;
}

void physics::updatePhysics (component::ComponentManager<game::AbstractEntity*>* componentManager) {
    componentManager->applyFunc(updatePhysicsInternal, 1,std::pair(componentManager->getComponent<CollisionComponent>(3),
                                        componentManager->getComponent<graphics::AbsolutePosition>(4)));
}

void physics::checkCollisions (component::ComponentManager<game::AbstractEntity*>* componentManager, event::EventBus* eventBus) {
    //logging::log(LEVEL_DEBUG, "Checking collisions!");
    std::vector<std::tuple<int,int, glm::vec2>> collisionResults; // TODO: do caching or something
    collisionResults.reserve(componentManager->getNumObjects());
    componentManager->applyFunc(checkCollisionsEntity, 3,  &collisionResults);
    for (auto p : collisionResults) {
        auto [x,y,dVec] = p;
        logging::logf(LEVEL_DEBUG, "Detected collision between entities %d and %d with min translation vec <%f, %f>!", x, y, dVec.x, dVec.y);
        // TODO: collision event
        auto comp1 = componentManager->getObjectData<CollisionComponent>(3, x);
        auto comp2 = componentManager->getObjectData<CollisionComponent>(3, y);
        auto comp1Mass = comp1.resolveLayers & comp2.layers ? comp1.mass : 0.0f;
        auto comp2Mass = comp2.resolveLayers & comp1.layers ? comp2.mass : 0.0f;
        float totalMass = comp1Mass + comp2Mass;
        if (totalMass != 0) {
            componentManager->getObjectDataPtr<component::EntityMainComponent>(1, x)->pos +=
                    dVec * comp1Mass / totalMass;
            componentManager->getObjectDataPtr<component::EntityMainComponent>(1, x)->vel -=
                    projectVec(dVec, componentManager->getObjectDataPtr<component::EntityMainComponent>(1, x)->vel);
            componentManager->getObjectDataPtr<component::EntityMainComponent>(1, y)->pos -=
                    dVec * comp2Mass / totalMass;
            componentManager->getObjectDataPtr<component::EntityMainComponent>(1, y)->vel -=
                    projectVec(dVec, componentManager->getObjectDataPtr<component::EntityMainComponent>(1, y)->vel);
        }
        if (comp1.layers & comp2.eventLayer) {
            eventBus->raiseEvent(event::EntityCollisionEvent(y, x, comp1.layers & comp2.eventLayer, componentManager));
        }
        if (comp2.layers & comp1.eventLayer) {
            eventBus->raiseEvent(event::EntityCollisionEvent(x, y, comp2.layers & comp1.eventLayer, componentManager));
        }
    }
}