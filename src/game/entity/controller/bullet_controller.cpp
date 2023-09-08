#include "engine/entity/controller/bullet_controller.h"
#include "physics/components/2D/rigid_body.h"
#include "game/entity/bullet.h"
#include "game/entity/serializers.h"
#include "physics/signals/2D/collision.h"
#include "component/component_serializer.h"

void game::BulletController::onEntityCollision (component::Entity& entityView, view::GameView& gameView, component::Entity& otherEntity, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.id());
}

void game::BulletController::initEntity (component::Entity& entityView, view::GameView& gameView, const util::DataValue& data) {
    entityView.emplace<game::Bullet>();
}

game::BulletController::BulletController () : EntityController("bullet") {}

void game::addBulletSignals (component::ComponentManager& manager, component::EntitySerializer& serialiser) {
    manager.addComponent<Bullet>();

    serialiser.addSerializer<Bullet>();

    manager.handleSignal<physics::OnCollision, Bullet>([] (const physics::OnCollision& signal, component::Entity entity, Bullet& bullet) {
        //info.manager().remove(info.id()); // TODO: queue
        //info.manager().entity(info.id()).remove();
        entity.remove();
    });
}