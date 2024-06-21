#include <phenyl/component.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/signals/physics.h>

#include "bullet.h"
#include "serializers.h"
#include "game/test_app.h"



using namespace phenyl;

void test::InitBullet (TestApp* app, phenyl::ComponentManager& manager) {
    app->addComponent<Bullet>();

    manager.handleSignal<physics::OnCollision, Bullet, const phenyl::GlobalTransform2D>([] (const physics::OnCollision& signal, phenyl::Entity entity, Bullet& bullet, const phenyl::GlobalTransform2D& transform) {
        GlobalTransform2D particleTransform{};
        particleTransform.transform2D
            .setPosition(transform.transform2D.position())
            .setRotation(transform.transform2D.rotationAngle());

        bullet.particlePrefab->instantiate()
            .with(particleTransform)
            .complete();

        entity.remove();
    });
}