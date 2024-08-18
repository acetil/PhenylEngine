#include <phenyl/component.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/signals/physics.h>

#include "bullet.h"
#include "serializers.h"
#include "game/test_app.h"



using namespace phenyl;

void test::InitBullet (TestApp* app, phenyl::ComponentManager& manager) {
    app->addComponent<Bullet>("Bullet");

    manager.addHandler<physics::OnCollision, Bullet, const phenyl::GlobalTransform2D>([] (const physics::OnCollision& signal, const phenyl::Bundle<Bullet, const phenyl::GlobalTransform2D>& bundle) {
        auto& [bullet, transform] = bundle.comps();

        GlobalTransform2D particleTransform{};
        particleTransform.transform2D
            .setPosition(transform.transform2D.position())
            .setRotation(transform.transform2D.rotationAngle());

        auto particleEntity = bundle.entity().manager().create();
        particleEntity.insert(particleTransform);
        bullet.particlePrefab->instantiate(particleEntity);

        bundle.entity().remove();
    });
}