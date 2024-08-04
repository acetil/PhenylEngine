#include <phenyl/components/audio_player.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/particle_emitter.h>
#include <phenyl/signals/physics.h>

#include "breakout.h"
#include "wall.h"


void breakout::InitWall (BreakoutApp* app, phenyl::ComponentManager& manager) {
    app->addComponent<Wall>();

    manager.handleSignal<phenyl::signals::OnCollision, phenyl::AudioPlayer, const Wall>([] (const phenyl::signals::OnCollision& signal, auto entity, phenyl::AudioPlayer& audioPlayer, const Wall& wall) {
        phenyl::GlobalTransform2D emitterTransform{};
        emitterTransform.transform2D.setPosition(signal.worldContactPoint);

        wall.emitter->instantiate()
            .with(emitterTransform)
            .complete()
            .apply<phenyl::ParticleEmitter2D>([normal=signal.normal] (phenyl::ParticleEmitter2D& emitter) {
                emitter.direction = normal;
            });
        audioPlayer.play(wall.bounceSample);
    });
}