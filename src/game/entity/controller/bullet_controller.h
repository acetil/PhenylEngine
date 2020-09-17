#include "entity_controller.h"
#ifndef BULLET_CONTROLLER_H
#define BULLET_CONTROLLER_H
namespace game {
    class BulletController : public EntityController {
    public:
        void onEntityCollision(view::EntityView &entityView, int otherEntityId, unsigned int layers) override;
    };
}
#endif //BULLET_CONTROLLER_H
