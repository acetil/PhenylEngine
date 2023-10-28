module;

#include <phenyl/serialization.h>

export module bullet;

//#include <phenyl/application.h>
//#include <phenyl/asset.h>
//#include <phenyl/component.h>
//#include <phenyl/prefab.h>

//#include <phenyl/components/2D/global_transform.h>
//#include <phenyl/signals/physics.h>

import phenyl.application;
import phenyl.asset;
import phenyl.component;
import phenyl.prefab;

import phenyl.components2d.global_transform;
import phenyl.signals.physics;

namespace test {
    struct Bullet {
        phenyl::Asset<phenyl::Prefab> particlePrefab;
    };

    PHENYL_SERIALIZE(Bullet, {
        PHENYL_MEMBER_NAMED(particlePrefab, "particle_prefab");
    })

    export void addBulletSignals (phenyl::Application* app, phenyl::ComponentManager& manager) {
        app->addComponent<Bullet>();

        manager.handleSignal<phenyl::signals::OnCollision, Bullet, const phenyl::GlobalTransform2D>([] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, Bullet& bullet, const phenyl::GlobalTransform2D& transform) {
            //info.manager().remove(info.id()); // TODO: queue
            //info.manager().entity(info.id()).remove();

            phenyl::GlobalTransform2D particleTransform{};
            particleTransform.transform2D
                             .setPosition(transform.transform2D.position())
                             .setRotation(transform.transform2D.rotationAngle());

            bullet.particlePrefab->instantiate()
                  .with(particleTransform)
                  .complete();

            entity.remove();
        });
    }
}