#include "graphics/particles/particle_system_2d.h"

#include "core/serialization/backends.h"
#include "core/serialization/serializer_impl.h"
#include "graphics/detail/loggers.h"
#include "util/random.h"

namespace phenyl::graphics {
PHENYL_SERIALIZABLE(ParticleProperties2D, PHENYL_SERIALIZABLE_MEMBER_NAMED(lifetimeMin, "lifetime_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(lifetimeMax, "lifetime_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(velocityMin, "velocity_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(velocityMax, "velocity_max"), PHENYL_SERIALIZABLE_MEMBER_NAMED(gravity, "gravity"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeStartMin, "size_start_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeStartMax, "size_start_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeEndMin, "size_end_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(sizeEndMax, "size_end_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(colourStartMin, "color_start_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(colourStartMax, "color_start_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(colourEndMin, "color_end_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(colourEndMax, "color_end_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(directionSpread, "spread"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(angleMin, "angle_min"), PHENYL_SERIALIZABLE_MEMBER_NAMED(angleMax, "angle_max"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(angularVelMin, "angular_vel_min"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(angularVelMax, "angular_vel_max"))
}

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"PARTICLE_SYSTEM2D", detail::GRAPHICS_LOGGER};

ParticleSystem2D::ParticleSystem2D (ParticleProperties2D properties, std::size_t maxParticles) :
    m_properties{properties},
    m_particles{maxParticles},
    m_startIndex{0},
    m_size{0},
    m_activeNum{0} {}

void ParticleSystem2D::emit (glm::vec2 worldPos, glm::vec2 direction) {
    if (m_size < m_particles.size()) {
        addParticle(worldPos, direction, (m_startIndex + m_size) % m_particles.size());
        m_size++;
        m_activeNum++;
    } else if (m_activeNum < m_particles.size()) {
        for (auto i = 0; i < m_particles.size(); i++) {
            auto index = (m_startIndex + i) % m_particles.size();
            if (!m_particles[index].active) {
                addParticle(worldPos, direction, index);
                m_activeNum++;
                return;
            }
        }

        // Evict first particle
        addParticle(worldPos, direction, m_startIndex);
    } else {
        // Evict first particle
        addParticle(worldPos, direction, m_startIndex);
        m_startIndex = (m_startIndex + 1) % m_particles.size();
    }
}

void ParticleSystem2D::addParticle (glm::vec2 worldPos, glm::vec2 direction, std::size_t index) {
    Particle& particle = m_particles[index];

    float dirAngle = std::atan2(direction.y, direction.x);
    float newDirAngle = util::Random::Rand(dirAngle - m_properties.directionSpread / 180 * std::numbers::pi_v<float>,
        dirAngle + m_properties.directionSpread / 180 * std::numbers::pi_v<float>);
    direction = glm::vec2{glm::cos(newDirAngle), glm::sin(newDirAngle)};

    particle.lifetime = util::Random::Rand(m_properties.lifetimeMin, m_properties.lifetimeMax);
    particle.remainingTime = particle.lifetime;

    particle.pos = worldPos;
    particle.vel = util::Random::Rand(m_properties.velocityMin, m_properties.velocityMax) * direction;
    particle.acc = m_properties.gravity;

    particle.colourStart = util::Random::Rand(m_properties.colourStartMin, m_properties.colourStartMax);
    particle.colourEnd = util::Random::Rand(m_properties.colourEndMin, m_properties.colourEndMax);
    particle.colour = particle.colourStart;

    particle.sizeStart = util::Random::Rand(m_properties.sizeStartMin, m_properties.sizeStartMax);
    particle.sizeEnd = util::Random::Rand(m_properties.sizeEndMin, m_properties.sizeEndMax);
    particle.size = particle.sizeStart;

    particle.angle = util::Random::Rand(m_properties.angleMin, m_properties.angleMax) / 180 * std::numbers::pi_v<float>;
    particle.angularVel =
        util::Random::Rand(m_properties.angularVelMin, m_properties.angularVelMax) / 180 * std::numbers::pi_v<float>;

    particle.active = true;
}

void ParticleSystem2D::update (float deltaTime) {
    auto beginSize = m_size;
    auto beginIndex = m_startIndex;
    for (std::size_t i = 0; i < beginSize; i++) {
        auto index = (beginIndex + i) % m_particles.size();

        auto& particle = m_particles[index];
        if (!particle.active) {
            continue;
        }

        particle.remainingTime -= deltaTime;
        if (particle.remainingTime <= 0) {
            particle.active = false;
            m_activeNum--;

            if (index == m_startIndex) {
                m_startIndex = (m_startIndex + 1) % m_particles.size();
                m_size--;
            } else if (index == (m_startIndex + m_size) % m_particles.size()) {
                while (m_size > 0 && !m_particles[(m_startIndex + m_size) % m_particles.size()].active) {
                    m_size--;
                }
            }
            continue;
        }

        particle.pos += particle.vel * deltaTime + 0.5f * particle.acc * deltaTime * deltaTime;
        particle.vel += particle.acc * deltaTime;

        particle.angle += particle.angularVel * deltaTime;

        particle.colour = (particle.colourEnd - particle.colourStart) * (particle.lifetime - particle.remainingTime) /
                particle.lifetime +
            particle.colourStart;
        particle.size =
            (particle.sizeEnd - particle.sizeStart) * (particle.lifetime - particle.remainingTime) / particle.lifetime +
            particle.sizeStart;
    }
}

void ParticleSystem2D::bufferPos (Buffer<glm::vec2>& buffer) const {
    static glm::vec2 vertices[] = {{-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f},
      {1.0f, -1.0f}};

    for (std::size_t i = 0; i < m_size; i++) {
        auto index = (m_startIndex + i) % m_particles.size();
        auto& particle = m_particles[index];

        if (!particle.active) {
            continue;
        }

        glm::mat2 mat = glm::mat2{{glm::cos(particle.angle), glm::sin(particle.angle)},
                          {-glm::sin(particle.angle), glm::cos(particle.angle)}} *
            glm::mat2{{particle.size, 0}, {0, particle.size}};

        for (auto v : vertices) {
            buffer.emplace(mat * v + particle.pos);
        }
    }
}

void ParticleSystem2D::bufferColour (Buffer<glm::vec4>& buffer) const {
    for (std::size_t i = 0; i < m_size; i++) {
        auto index = (m_startIndex + i) % m_particles.size();
        auto& particle = m_particles[index];

        if (!particle.active) {
            continue;
        }

        for (auto j = 0; j < 6; j++) {
            buffer.emplace(particle.colour);
        }
    }
}

std::string_view ParticleSystem2D::GetAssetType () {
    return "phenyl::ParticleSystem2D";
}

std::optional<ParticleProperties2D> phenyl::graphics::LoadParticleProperties2D (std::istream& file) {
    /*nlohmann::json json;
    file >> json;
    phenyl::common::JsonDeserializer deserializer{json};

    return deserializer.deserialize<ParticleProperties2D>();*/
    try {
        return phenyl::core::DeserializeFromJson<ParticleProperties2D>(file);
    } catch (const DeserializeException& e) {
        PHENYL_LOGE(LOGGER, "Failed to deserialize particle properties: {}", e.what());
        return std::nullopt;
    }
}
