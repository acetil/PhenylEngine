#pragma once

#include "engine/entity/entity.h"
#include "graphics/textures/texture_atlas.h"

namespace game {
    class EntityTest : public AbstractEntity {
        public:
        EntityTest();
        virtual AbstractEntity* createEntity();
        void setTextureIds (graphics::TextureAtlas& atlas) override;
    };
}
