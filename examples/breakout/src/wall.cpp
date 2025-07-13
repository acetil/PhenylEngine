#include "wall.h"

#include "breakout.h"

#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/particle_emitter.h>
#include <phenyl/components/audio_player.h>
#include <phenyl/signals/physics.h>

namespace breakout {
PHENYL_SERIALIZABLE(Wall, PHENYL_SERIALIZABLE_MEMBER(emitter),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(bounceSample, "bounce_sample"))
}

void breakout::InitWall (BreakoutApp* app, phenyl::World& world) {
    app->addComponent<Wall>("Wall");

    world.addHandler<phenyl::signals::OnCollision, phenyl::AudioPlayer, const Wall>(
        [] (const phenyl::signals::OnCollision& signal, const phenyl::Bundle<phenyl::AudioPlayer, const Wall>& bundle) {
            auto& [audioPlayer, wall] = bundle.comps();
            auto& world = bundle.entity().world();

            auto emitterEntity = world.create();
            emitterEntity.insert(phenyl::Transform2D{}.setPosition(signal.worldContactPoint));
            wall.emitter->instantiate(emitterEntity);
            emitterEntity.apply<phenyl::ParticleEmitter2D>(
                [normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) { emitter.direction = normal; });

            audioPlayer.play(*wall.bounceSample);
            /*wall.emitter->instantiate()
                .with(emitterTransform)
                .complete()
                .apply<phenyl::ParticleEmitter2D>([normal=signal.normal]
            (phenyl::ParticleEmitter2D& emitter) { emitter.direction = normal;
                });
            audioPlayer.play(wall.bounceSample);*/
        });
}
