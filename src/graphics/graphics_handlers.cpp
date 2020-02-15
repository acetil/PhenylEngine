#include "graphics_handlers.h"
#include "event/events/entity_creation.h"
#include "game/entity/entity.h"
using namespace graphics;

void graphics::onEntityCreation (event::EntityCreationEvent* event) {
    int texId = event->entity->getTextureId();
    unsigned int id = event->entityId;
    float* pointer = event->compManager->getEntityDataPtr<float>(2, id);
    TextureAtlas* atlas = event->graphics->getTextureAtlas();
    Texture* tex = atlas->getTexture(texId);
    memcpy(pointer, tex->getTexUvs(), 12 * sizeof(float));
}