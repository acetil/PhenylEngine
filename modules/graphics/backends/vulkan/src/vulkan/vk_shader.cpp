#include "vk_shader.h"

#include "core/assets/assets.h"

using namespace phenyl::vulkan;

std::size_t VulkanShader::hash () const noexcept {
    return 1;
}

std::optional<unsigned int> VulkanShader::getAttribLocation (const std::string& attrib) const noexcept {
    return 0;
}

std::optional<unsigned int> VulkanShader::getUniformLocation (const std::string& uniform) const noexcept {
    return 0;
}

std::optional<unsigned int> VulkanShader::getSamplerLocation (const std::string& sampler) const noexcept {
    return  0;
}

std::optional<std::size_t> VulkanShader::getUniformOffset (const std::string& uniformBlock,
    const std::string& uniform) const noexcept {
    return 0;
}

std::optional<std::size_t> VulkanShader::getUniformBlockSize (const std::string& uniformBlock) const noexcept {
    return 0;
}

phenyl::graphics::Shader* VulkanShaderManager::load (std::ifstream& data, std::size_t id) {
    PHENYL_ABORT("Unimplemented");
}

phenyl::graphics::Shader* VulkanShaderManager::load (graphics::Shader&& obj, std::size_t id) {
    shaders.emplace(id, std::make_unique<graphics::Shader>(std::move(obj)));

    return shaders[id].get();
}

void VulkanShaderManager::queueUnload (std::size_t id) {

}

bool VulkanShaderManager::isBinary () const {
    return false;
}

const char* VulkanShaderManager::getFileType () const {
    return ""; // TODO
}

void VulkanShaderManager::selfRegister () {
    core::Assets::AddManager(this);
}

void VulkanShaderManager::clearDefaults () {
    defaultShaders.clear();
}

void VulkanShaderManager::loadDefaultShaders () {
    loadDefault("phenyl/shaders/box", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/debug", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/sprite", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/canvas", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/particle", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/blinn_phong", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/shadow_map", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/mesh_prepass", std::make_unique<VulkanShader>());
    loadDefault("phenyl/shaders/postprocess/noop", std::make_unique<VulkanShader>());
}

void VulkanShaderManager::loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader) {
    defaultShaders.emplace_back(core::Assets::LoadVirtual(path, graphics::Shader{std::move(shader)}));
}