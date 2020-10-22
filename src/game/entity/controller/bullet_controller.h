#include "entity_controller.h"
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
    };
}
#endif //BULLET_CONTROLLER_H
