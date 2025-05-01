#include "core/assets/assets.h"

#include "graphics/particles/particle_manager.h"

const char* phenyl::graphics::ParticleManager2D::getFileType () const {
    return ".json";
}

phenyl::graphics::ParticleSystem2D* phenyl::graphics::ParticleManager2D::load (std::ifstream& data, std::size_t id) {
    auto propOpt = LoadParticleProperties2D(data);
    if (!propOpt) {
        return nullptr;
    }

    auto prop = propOpt.getUnsafe();

    m_systems[id] = std::make_unique<ParticleSystem2D>(prop, m_maxParticles);

    return m_systems[id].get();
}

phenyl::graphics::ParticleManager2D::ParticleManager2D (std::size_t systemMaxParticles) : m_maxParticles{systemMaxParticles} {}

phenyl::graphics::ParticleSystem2D* phenyl::graphics::ParticleManager2D::load (phenyl::graphics::ParticleSystem2D&& obj, std::size_t id) {
    m_systems[id] = std::make_unique<ParticleSystem2D>(std::move(obj));

    return m_systems[id].get();
}

void phenyl::graphics::ParticleManager2D::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        m_systems.remove(id);
    }
}

void phenyl::graphics::ParticleManager2D::update (float deltaTime) {
    for (auto [_, system] : m_systems.kv()) {
        system->update(deltaTime);
    }
}

void phenyl::graphics::ParticleManager2D::buffer (phenyl::graphics::Buffer<glm::vec2>& posBuffer, phenyl::graphics::Buffer<glm::vec4>& colourBuffer) const {
    for (auto [_, system] : m_systems.kv()) {
        system->bufferPos(posBuffer);
    }

    for (auto [_, system] : m_systems.kv()) {
        system->bufferColour(colourBuffer);
    }
}

std::string_view phenyl::graphics::ParticleManager2D::getName () const noexcept {
    return "ParticleManager2D";
}

void phenyl::graphics::ParticleManager2D::selfRegister () {
    phenyl::core::Assets::AddManager(this);
}
