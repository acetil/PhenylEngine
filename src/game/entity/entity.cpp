#include "entity.h"
#include "logging/logging.h"
using namespace game;

void game::AbstractEntity::setEntityId (int entityId) {
    this->entityId = entityId;
}
int game::AbstractEntity::getEntityId () {
    return entityId;
}
std::string game::AbstractEntity::getEntityName () {
    return name;
}
void game::AbstractEntity::update (float time) {
    // default is empty
}
int game::AbstractEntity::getTextureId () {
    return textureId;
}
// graphics code for base class

/*void game::AbstractEntity::render (graphics::Graphics* graphics) {
    graphics->drawTexSquare(*x, *y, scale, textureId);
}*/
EntityController* game::AbstractEntity::getController() {
    return controller;
}
