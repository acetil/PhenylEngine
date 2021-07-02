#include "entity.h"
#ifndef BASIC_ENTITY_H
#define BASIC_ENTITY_H
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
#endif //BASIC_ENTITY_H
