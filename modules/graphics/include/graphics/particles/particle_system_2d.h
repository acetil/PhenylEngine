#pragma once

#include <iosfwd>
#include <random>
#include <vector>

#include "graphics/maths_headers.h"
#include "util/optional.h"
#include "graphics/backend/buffer.h"

namespace phenyl::graphics {
    struct ParticleProperties2D {
        float lifetimeMin;
        float lifetimeMax;

        float velocityMin;
        float velocityMax;

        glm::vec2 gravity;
        float directionSpread;

        float sizeStartMin;
        float sizeStartMax;

        float sizeEndMin;
        float sizeEndMax;

        float angleMin;
        float angleMax;

        float angularVelMin;
        float angularVelMax;

        glm::vec4 colourStartMin;
        glm::vec4 colourStartMax;

        glm::vec4 colourEndMin;
        glm::vec4 colourEndMax;
    };

    class ParticleSystem2D {
    public:
        ParticleSystem2D (ParticleProperties2D properties, std::size_t maxParticles);

        void emit (glm::vec2 worldPos, glm::vec2 direction);

        void update (float deltaTime);

        void bufferPos (Buffer<glm::vec2>& buffer) const;
        void bufferColour (Buffer<glm::vec4>& buffer) const;

    private:
        struct Particle {
            glm::vec2 pos{0, 0};
            glm::vec2 vel{0, 0};
            glm::vec2 acc{0, 0};

            float sizeStart;
            float sizeEnd;
            float size{0};

            float angle;
            float angularVel;

            glm::vec4 colourStart{0, 0, 0, 0};
            glm::vec4 colourEnd{0, 0, 0, 0};
            glm::vec4 colour;

            float lifetime{0};
            float remainingTime{0};

            bool active = false;
        };

        ParticleProperties2D m_properties;

        std::vector<Particle> m_particles;
        std::size_t m_startIndex;
        std::size_t m_size;
        std::size_t m_activeNum;

        void addParticle (glm::vec2 worldPos, glm::vec2 direction, std::size_t index);
    };

    util::Optional<ParticleProperties2D> LoadParticleProperties2D (std::istream& file);
}
