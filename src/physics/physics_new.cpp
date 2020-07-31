#include "physics_new.h"
#include "math.h"

using namespace physics;
void physics::updatePhysics (component::EntityMainComponent* comp, int numEntities, int direction) {
    // TODO: consider writing fully in assembly
    for (int i = 0; i < numEntities; i ++) {
        int isPosXVel = comp->vel[0] > 0;
        int isPosYVel = comp->vel[1] > 0;
        //logging::logf(LEVEL_DEBUG, "Current pos before: (%f, %f). Current vel: (%f, %f).", comp->pos[0], comp->pos[1], comp->vel[0], comp->vel[1]);
        comp->vel[0] -= ((float)comp->vel[0] * comp->linFriction) + comp->constFriction * (isPosXVel * 2 - 1);
        comp->vel[1] -= ((float)comp->vel[1] * comp->linFriction) + comp->constFriction * (isPosYVel * 2 - 1);

        comp->vel[0] *= (comp->vel[0] > 0 && isPosXVel) || (comp->vel[0] < 0 && !isPosXVel);
        comp->vel[1] *= (comp->vel[1] > 0 && isPosYVel) || (comp->vel[1] < 0 && !isPosYVel);

        comp->vel[0] += comp->acc[0];
        comp->vel[1] += comp->acc[1];

        comp->pos[0] += comp->vel[0];
        comp->pos[1] += comp->vel[1];
        comp += direction;
        //logging::logf(LEVEL_DEBUG, "Current pos: (%f, %f). Current vel: (%f, %f).", comp->pos[0], comp->pos[1], comp->vel[0], comp->vel[1]);
    }
}
void physics::onEntityCreation (event::EntityCreationEvent* event) {
    logging::log(LEVEL_DEBUG, "About to get main component!");
    component::EntityMainComponent* comp = event->compManager->getObjectDataPtr<component::EntityMainComponent>(1, event->entityId);
    comp->pos[0] = event->x;
    comp->pos[1] = event->y;
    comp->vel[0] = 0;
    comp->vel[1] = 0;
    comp->vec1[0] = event->size;
    comp->vec2[1] = event->size;
    comp->acc[0] = 0;
    comp->acc[1] = 0;
    comp->linFriction = 0.2;
    comp->constFriction = 0.002; // TODO: remove hardcoding and add EntityType etc.
}