#include "entity_bullet.h"
#include "controller/bullet_controller.h"

using namespace game;

EntityBullet::EntityBullet () {
    scale = 0.03f;
    name = "bullet";
    //controller = new BulletController();
}

AbstractEntity* EntityBullet::createEntity () {
    auto entity = new EntityBullet();
    entity->textureId = textureId;
    //entity->controller = controller;
    return entity;
}

void EntityBullet::setTextureIds (graphics::TextureAtlas &atlas) {
    textureId = atlas.getModelId("test9");
}

