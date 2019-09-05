#include "entity.h"
#include "graphics/textures/texture_atlas.h"
#ifndef ENTITY_TEST_H
#define ENTITY_TEST_H
namespace game {
    class EntityTest : public AbstractEntity {
        public:
        EntityTest();
        virtual AbstractEntity* createEntity(float x, float y);
        virtual void setTextureIds (graphics::TextureAtlas* atlas);
    };
}
#endif