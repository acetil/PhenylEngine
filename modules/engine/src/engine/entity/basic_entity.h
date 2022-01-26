#pragma once

#include "engine/entity/entity.h"

namespace game {
    class BasicEntity : public AbstractEntity {
    public:
        AbstractEntity* createEntity () override {
            return new BasicEntity();
        };

        void setTextureIds (graphics::TextureAtlas& graphics) override {};
        //virtual void render (graphics::Graphics* graphics);
    };
}