#include "physics_new.h"
#include "math.h"

using namespace physics;
void physics::updatePhysics (component::EntityMainComponent* comp, int numEntities, int direction) {
    // TODO: consider writing fully in assembly
    for (int i = 0; i < numEntities; i ++) {
        int isPosXVel = comp->vel[0] > 0;
        int isPosYVel = comp->vel[1] > 0;

        comp->vel[0] -= (ceil((float)comp->vel[0] * comp->linFriction) + comp->constFriction) * (isPosXVel * 2 - 1);
        comp->vel[1] -= (ceil((float)comp->vel[1] * comp->linFriction) + comp->constFriction) * (isPosYVel * 2 - 1);

        comp->vel[0] *= (comp->vel[0] > 0 && isPosXVel);
        comp->vel[1] *= (comp->vel[1] > 0 && isPosYVel);

        comp->vel[0] += comp->acc[0];
        comp->vel[1] += comp->acc[1];

        comp->pos[0] += comp->vel[0];
        comp->pos[1] += comp->vel[1];
        comp += direction;
    }
}
void physics::onEntityCreation (event::EntityCreationEvent* event) {
    logging::log(LEVEL_DEBUG, "About to get main component!");
    component::EntityMainComponent* comp = event->compManager->getEntityDataPtr<component::EntityMainComponent>(1, event->entityId);
    comp->pos[0] = event->x;
    comp->pos[1] = event->y;
    comp->vec1[0] = event->size;
    comp->vec2[1] = event->size;
}