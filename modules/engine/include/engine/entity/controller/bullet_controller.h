#include "engine/entity/controller/entity_controller.h"
#ifndef BULLET_CONTROLLER_H
#define BULLET_CONTROLLER_H
namespace game {
    class BulletController : public EntityController {
    private:
        int texId = 0;
    public:
        void onEntityCollision(view::EntityView &entityView, view::GameView& gameView, int otherEntityId, unsigned int layers) override;
        int getTextureId (view::EntityView& entityView, view::GameView& gameView) override;
        void setTextureIds (graphics::TextureAtlas& atlas) override;
        void initEntity (view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) override;
    };
}
#endif //BULLET_CONTROLLER_H
