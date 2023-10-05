#include <nlohmann/json.hpp>

#include "common/serializer.h"
#include "common/maths/glm_serializer.h"
#include "util/random.h"

#include "graphics/particles/particle_system_2d.h"

namespace phenyl::graphics {
    PHENYL_SERIALIZE(ParticleProperties2D, {
        PHENYL_MEMBER_NAMED(lifetimeMin, "lifetime_min");
        PHENYL_MEMBER_NAMED(lifetimeMax, "lifetime_max");
        PHENYL_MEMBER_NAMED(velocityMin, "velocity_min");
        PHENYL_MEMBER_NAMED(velocityMax, "velocity_max");
        PHENYL_MEMBER_NAMED(gravity, "gravity");
        PHENYL_MEMBER_NAMED(sizeMin, "size_min");
        PHENYL_MEMBER_NAMED(sizeMax, "size_max");
        PHENYL_MEMBER_NAMED(colourStart, "color_start");
        PHENYL_MEMBER_NAMED(colourEnd, "color_end");
    })
}

using namespace phenyl::graphics;

ParticleSystem2D::ParticleSystem2D (ParticleProperties2D properties, std::size_t maxParticles) : properties{properties}, particles{maxParticles}, startIndex{0}, size{0}, activeNum{0} {}

void ParticleSystem2D::emit (glm::vec2 worldPos, glm::vec2 direction) {
    if (size < particles.size()) {
        addParticle(worldPos, direction, (startIndex + size) % particles.size());
        size++;
        activeNum++;
    } else if (activeNum < particles.size()) {
        for (auto i = 0; i < particles.size(); i++) {
            auto index = (startIndex + i) % particles.size();
            if (!particles[index].active) {
                addParticle(worldPos, direction, index);
                activeNum++;
                return;
            }
        }

        // Evict first particle
        addParticle(worldPos, direction, startIndex);
    } else {
        // Evict first particle
        addParticle(worldPos, direction, startIndex);
    }
}

void ParticleSystem2D::addParticle (glm::vec2 worldPos, glm::vec2 direction, std::size_t index) {
    Particle& particle = particles[index];

    particle.lifetime = util::Random::Rand(properties.lifetimeMin, properties.lifetimeMax);
    particle.remainingTime = particle.lifetime;

    particle.pos = worldPos;
    particle.vel = util::Random::Rand(properties.velocityMin, properties.velocityMax) * direction;
    particle.acc = properties.gravity;

    particle.colourStart = properties.colourStart;
    particle.colourEnd = properties.colourEnd;
    particle.colour = properties.colourStart;

    particle.size = util::Random::Rand(properties.sizeMin, properties.sizeMax);

    particle.active = true;
}

void ParticleSystem2D::update (float deltaTime) {
    auto beginSize = size;
    auto beginIndex = startIndex;
    for (std::size_t i = 0; i < beginSize; i++) {
        auto index = (beginIndex + i) % particles.size();

        auto& particle = particles[index];
        particle.remainingTime -= deltaTime;
        if (particle.remainingTime <= 0) {
            particle.active = false;
            activeNum--;

            if (index == startIndex) {
                startIndex = (startIndex + 1) % particles.size();
                size--;
            } else if (index == (startIndex + size) % particles.size()) {
                size--;
            }
            continue;
        }

        particle.pos += particle.vel * deltaTime + 0.5f * particle.acc * deltaTime * deltaTime;
        particle.vel += particle.acc * deltaTime;
        particle.colour = (particle.colourEnd - particle.colourStart) * (particle.lifetime - particle.remainingTime) / particle.lifetime + particle.colourStart;
    }
}

void ParticleSystem2D::bufferPos (Buffer<glm::vec2>& buffer) const {
    static glm::vec2 vertices[] = {
            {-1.0f, -1.0f},
            {1.0f, -1.0f},
            {-1.0f, 1.0f},
            {1.0f, 1.0f},
            {-1.0f, 1.0f},
            {1.0f, -1.0f}
    };

    for (std::size_t i = 0; i < size; i++) {
        auto index = (startIndex + i) % particles.size();
        auto& particle = particles[index];

        if (!particle.active) {
            continue;
        }

        for (auto v : vertices) {
            buffer.pushData(v * particle.size + particle.pos);
        }
    }
}

void ParticleSystem2D::bufferColour (Buffer<glm::vec4>& buffer) const {
    for (std::size_t i = 0; i < size; i++) {
        auto index = (startIndex + i) % particles.size();
        auto& particle = particles[index];

        if (!particle.active) {
            continue;
        }

        for (auto j = 0; j < 6; j++) {
            buffer.pushData(particle.colour);
        }
    }
}

phenyl::util::Optional<ParticleProperties2D> phenyl::graphics::LoadParticleProperties2D (std::istream& file) {
    nlohmann::json json;
    file >> json;
    phenyl::common::JsonDeserializer deserializer{json};

    return deserializer.deserialize<ParticleProperties2D>();
}