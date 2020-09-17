#include "entity.h"
#ifndef ENTITY_BULLET_H
#define ENTITY_BULLET_H
namespace game {
    class EntityBullet : public AbstractEntity {
    public:
        EntityBullet ();
        AbstractEntity* createEntity() override;
        void setTextureIds (graphics::TextureAtlas& atlas) override;
    };
}
#endif
