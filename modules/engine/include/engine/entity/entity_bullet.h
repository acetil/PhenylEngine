#pragma once

#include "engine/entity/entity.h"

namespace game {
    class EntityBullet : public AbstractEntity {
    public:
        EntityBullet ();
        AbstractEntity* createEntity() override;
        void setTextureIds (graphics::TextureAtlas& atlas) override;
    };
}
