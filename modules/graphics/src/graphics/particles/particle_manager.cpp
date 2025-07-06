#include "graphics/particles/particle_manager.h"

#include "core/assets/assets.h"

#include <ranges>

const char* phenyl::graphics::ParticleManager2D::getFileType () const {
    return ".json";
}

std::shared_ptr<phenyl::graphics::ParticleSystem2D> phenyl::graphics::ParticleManager2D::load (std::ifstream& data) {
    auto propOpt = LoadParticleProperties2D(data);
    if (!propOpt) {
        return nullptr;
    }
    auto system = std::make_shared<ParticleSystem2D>(*propOpt, m_maxParticles);
    m_systems.emplace_back(system);
    return system;
}

phenyl::graphics::ParticleManager2D::ParticleManager2D (std::size_t systemMaxParticles) :
    m_maxParticles{systemMaxParticles} {}

void phenyl::graphics::ParticleManager2D::update (float deltaTime) {
    for (const auto& system : m_systems) {
        if (auto ptr = system.lock()) {
            ptr->update(deltaTime);
        }
    }

    std::erase_if(m_systems, [] (const auto& ptr) { return ptr.expired(); });
}

void phenyl::graphics::ParticleManager2D::buffer (phenyl::graphics::Buffer<glm::vec2>& posBuffer,
    phenyl::graphics::Buffer<glm::vec4>& colourBuffer) const {
    for (const auto& system : m_systems) {
        if (auto ptr = system.lock()) {
            ptr->bufferPos(posBuffer);
            ptr->bufferColour(colourBuffer);
        }
    }
}

std::string_view phenyl::graphics::ParticleManager2D::getName () const noexcept {
    return "ParticleManager2D";
}

void phenyl::graphics::ParticleManager2D::selfRegister () {
    phenyl::core::Assets::AddManager(this);
}
