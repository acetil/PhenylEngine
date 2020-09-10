#include "graphics_update.h"
using namespace graphics;

void graphics::updateEntityRotation (event::EntityRotationEvent &event) {
    auto ptr = event.manager->getObjectDataPtr<AbsolutePosition>(event.entityId);
    ptr->transform *= event.rotMatrix;
}