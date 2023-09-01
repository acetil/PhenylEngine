#include "engine/entity/controller/bullet_controller.h"
#include "physics/components/2D/rigid_body.h"
#include "game/entity/bullet.h"
#include "physics/signals/2D/collision.h"
#include "component/component_serialiser.h"

void game::BulletController::onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.id());
}

void game::BulletController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    entityView.insert<game::Bullet>();
}

game::BulletController::BulletController () : EntityController("bullet") {}

void game::addBulletSignals (component::ComponentManager& manager, component::EntitySerialiser& serialiser) {
    manager.addComponent<Bullet>();

    serialiser.addComponentSerialiser<Bullet>("Bullet", [] (const Bullet& bullet) {
        return (util::DataValue)util::DataObject{};
    }, [] (const util::DataValue& val) {
        return val.is<util::DataObject>() ? util::Optional<Bullet>{Bullet{}} : util::Optional<Bullet>{};
    });

    manager.handleSignal<physics::OnCollision, Bullet>([] (const physics::OnCollision& signal, component::IterInfo& info, Bullet& bullet) {
        info.manager().remove(info.id()); // TODO: queue
    });
}