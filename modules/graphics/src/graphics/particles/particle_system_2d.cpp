#include <nlohmann/json.hpp>

#include "common/serializer_impl.h"
#include "util/random.h"

#include "graphics/particles/particle_system_2d.h"

namespace phenyl::graphics {
    PHENYL_SERIALIZABLE(ParticleProperties2D,
        PHENYL_SERIALIZABLE_MEMBER_NAMED(lifetimeMin, "lifetime_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(lifetimeMax, "lifetime_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(velocityMin, "velocity_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(velocityMax, "velocity_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(gravity, "gravity"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeStartMin, "size_start_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeStartMax, "size_start_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeEndMin, "size_end_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeEndMax, "size_end_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(colourStartMin, "color_start_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(colourStartMax, "color_start_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(colourEndMin, "color_end_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(colourEndMax, "color_end_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(directionSpread, "spread"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(angleMin, "angle_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(angleMax, "angle_max"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(angularVelMin, "angular_vel_min"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(angularVelMax, "angular_vel_max"))
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
        startIndex = (startIndex + 1) % particles.size();
    }
}

void ParticleSystem2D::addParticle (glm::vec2 worldPos, glm::vec2 direction, std::size_t index) {
    Particle& particle = particles[index];

    float dirAngle = std::atan2(direction.y, direction.x);
    float newDirAngle = util::Random::Rand(dirAngle - properties.directionSpread / 180 * std::numbers::pi_v<float>, dirAngle + properties.directionSpread / 180 * std::numbers::pi_v<float>);
    direction = glm::vec2{glm::cos(newDirAngle), glm::sin(newDirAngle)};

    particle.lifetime = util::Random::Rand(properties.lifetimeMin, properties.lifetimeMax);
    particle.remainingTime = particle.lifetime;

    particle.pos = worldPos;
    particle.vel = util::Random::Rand(properties.velocityMin, properties.velocityMax) * direction;
    particle.acc = properties.gravity;

    particle.colourStart = util::Random::Rand(properties.colourStartMin, properties.colourStartMax);
    particle.colourEnd = util::Random::Rand(properties.colourEndMin, properties.colourEndMax);
    particle.colour = particle.colourStart;

    particle.sizeStart = util::Random::Rand(properties.sizeStartMin, properties.sizeStartMax);
    particle.sizeEnd = util::Random::Rand(properties.sizeEndMin, properties.sizeEndMax);
    particle.size = particle.sizeStart;

    particle.angle = util::Random::Rand(properties.angleMin, properties.angleMax) / 180 * std::numbers::pi_v<float>;
    particle.angularVel = util::Random::Rand(properties.angularVelMin, properties.angularVelMax) / 180 * std::numbers::pi_v<float>;

    particle.active = true;
}

void ParticleSystem2D::update (float deltaTime) {
    auto beginSize = size;
    auto beginIndex = startIndex;
    for (std::size_t i = 0; i < beginSize; i++) {
        auto index = (beginIndex + i) % particles.size();

        auto& particle = particles[index];
        if (!particle.active) {
            continue;
        }

        particle.remainingTime -= deltaTime;
        if (particle.remainingTime <= 0) {
            particle.active = false;
            activeNum--;

            if (index == startIndex) {
                startIndex = (startIndex + 1) % particles.size();
                size--;
            } else if (index == (startIndex + size) % particles.size()) {
                while (size > 0 && !particles[(startIndex + size) % particles.size()].active) {
                    size--;
                }
            }
            continue;
        }

        particle.pos += particle.vel * deltaTime + 0.5f * particle.acc * deltaTime * deltaTime;
        particle.vel += particle.acc * deltaTime;

        particle.angle += particle.angularVel * deltaTime;

        particle.colour = (particle.colourEnd - particle.colourStart) * (particle.lifetime - particle.remainingTime) / particle.lifetime + particle.colourStart;
        particle.size = (particle.sizeEnd - particle.sizeStart) * (particle.lifetime - particle.remainingTime) / particle.lifetime + particle.sizeStart;
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

        glm::mat2 mat = glm::mat2{{glm::cos(particle.angle), glm::sin(particle.angle)}, {-glm::sin(particle.angle), glm::cos(particle.angle)}} * glm::mat2{{particle.size, 0}, {0, particle.size}};

        for (auto v : vertices) {
            buffer.emplace(mat * v + particle.pos);
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
            buffer.emplace(particle.colour);
        }
    }
}

phenyl::util::Optional<ParticleProperties2D> phenyl::graphics::LoadParticleProperties2D (std::istream& file) {
    /*nlohmann::json json;
    file >> json;
    phenyl::common::JsonDeserializer deserializer{json};

    return deserializer.deserialize<ParticleProperties2D>();*/
    return util::NullOpt;
}