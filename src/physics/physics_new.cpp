#include "physics_new.h"
#include "math.h"
#include "collisions.h"
#include <tuple>

using namespace physics;

void updatePhysicsInternal (component::EntityMainComponent* comp, int numEntities, int direction, physics::CollisionComponent* comp2) {
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

        comp2->pos = comp->pos;
        comp += direction;
        comp2 += direction;
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
    comp->vec1[0] = event.size;
    comp->vec2[1] = event.size;
    comp->acc[0] = 0;
    comp->acc[1] = 0;
    comp->linFriction = 0.2;
    comp->constFriction = 0.002; // TODO: remove hardcoding and add EntityType etc.
    auto comp2 = event.compManager->getObjectDataPtr<CollisionComponent>(3, event.entityId);
    comp2->pos = comp->pos;
    comp2->bbMap = {{event.size / 2, 0.0f}, {0.0f, event.size / 2}};
    comp2->outerRadius = sqrt(2) * event.size;
}

void physics::updatePhysics (component::ComponentManager<game::AbstractEntity*>* componentManager) {
    componentManager->applyFunc(updatePhysicsInternal, 1, componentManager->getComponent<CollisionComponent>(3));
}

void physics::checkCollisions (component::ComponentManager<game::AbstractEntity*>* componentManager) {
    //logging::log(LEVEL_DEBUG, "Checking collisions!");
    std::vector<std::tuple<int,int, glm::vec2>> collisionResults; // TODO: do caching or something
    collisionResults.reserve(componentManager->getNumObjects());
    componentManager->applyFunc(checkCollisionsEntity, 3,  &collisionResults);
    for (auto p : collisionResults) {
        auto [x,y,dVec] = p;
        //logging::logf(LEVEL_DEBUG, "Detected collision between entities %d and %d with min translation vec <%f, %f>!", x, y, dVec.x, dVec.y);
        // TODO: better collision resolving code
        componentManager->getObjectDataPtr<component::EntityMainComponent>(1, x)->pos += dVec;
        componentManager->getObjectDataPtr<component::EntityMainComponent>(1, x)->vel = {0,0};
    }
}