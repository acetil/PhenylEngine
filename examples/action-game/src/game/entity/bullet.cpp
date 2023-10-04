#include <phenyl/component.h>
#include <phenyl/signals/physics.h>

#include "bullet.h"
#include "serializers.h"
#include "game/test_app.h"


using namespace phenyl;

void test::addBulletSignals (TestApp* app, phenyl::ComponentManager& manager) {
    app->addComponent<Bullet>();

    manager.handleSignal<physics::OnCollision, Bullet>([] (const physics::OnCollision& signal, phenyl::Entity entity, Bullet& bullet) {
        //info.manager().remove(info.id()); // TODO: queue
        //info.manager().entity(info.id()).remove();
        entity.remove();
    });
}