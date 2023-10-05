#pragma once

#include "common/assets/asset.h"
#include "component/forward.h"
#include "graphics/maths_headers.h"

namespace phenyl::common {
    class GlobalTransform2D;
}

namespace phenyl::graphics {
    class ParticleSystem2D;

    struct ParticleEmitter2D {
    public:
        phenyl::common::Asset<ParticleSystem2D> system;
        glm::vec2 direction{1.0f, 0.0f};

        float duration;
        float explosiveness;

        std::size_t particlesPerLoop;
        std::size_t particlesEmitted{0};
        float timeInLoop{0.0f};

        bool oneShot = false;
        bool enabled = true;

        static void Update (float deltaTime, phenyl::component::ComponentManager& componentManager);
        void start ();
    };
}