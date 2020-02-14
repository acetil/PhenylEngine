#include "graphics_handlers.h"
#include "event/events/entity_creation.h"
#include "game/entity/entity.h"
using namespace graphics;

void graphics::onEntityCreation (event::EntityCreationEvent* event) {
    int texId = event->entity->getTextureId();
    unsigned int id = event->entityId;
    logging::logf(LEVEL_DEBUG, "Beginning copying of uvs at id %d! graphics == null? %d", texId, event->graphics == nullptr);
    float* pointer = event->compManager->getEntityDataPtr<float>(2, id);
    logging::log(LEVEL_DEBUG, "Here!");
    TextureAtlas* atlas = event->graphics->getTextureAtlas();
    logging::log(LEVEL_DEBUG, "Here!");
    Texture* tex = atlas->getTexture(texId);
    logging::log(LEVEL_DEBUG, "Here!");
    memcpy(pointer, tex->getTexUvs(), 12 * sizeof(float));
    logging::log(LEVEL_DEBUG, "Ending copying of uvs!");
}