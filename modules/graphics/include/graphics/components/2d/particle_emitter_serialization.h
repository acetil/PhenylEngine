#pragma once

#include "particle_emitter.h"
#include "common/serializer.h"
#include "common/assets/asset_serializer.h"
#include "common/maths/glm_serializer.h"

namespace phenyl::graphics {
    PHENYL_SERIALIZE(ParticleEmitter2D, {
        PHENYL_MEMBER(system);
        PHENYL_MEMBER(direction);
        PHENYL_MEMBER(duration);
        PHENYL_MEMBER(explosiveness);
        PHENYL_MEMBER_NAMED(particlesPerLoop, "num_particles");
        PHENYL_MEMBER_NAMED(oneShot, "one_shot");
        PHENYL_MEMBER(enabled);
    })
}