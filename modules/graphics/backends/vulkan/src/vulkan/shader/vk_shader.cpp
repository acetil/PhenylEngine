#include "vk_shader.h"

#include "core/assets/assets.h"

#include <ranges>

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_SHADER", detail::VULKAN_LOGGER};

static VkShaderStageFlagBits ConvertShaderType (phenyl::graphics::ShaderSourceType shaderType) {
    switch (shaderType) {
    case phenyl::graphics::ShaderSourceType::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    case phenyl::graphics::ShaderSourceType::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    }

    PHENYL_ABORT("Unexpected shader type: {}", static_cast<std::uint32_t>(shaderType));
}

VulkanShader::VulkanShader (VkDevice device, std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules,
    ShaderReflection reflection) :
    m_device{device},
    m_modules{std::move(modules)},
    m_reflection{std::move(reflection)} {}

std::unique_ptr<VulkanShader> VulkanShader::Make (VkDevice device,
    const std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>>& sources,
    const std::vector<std::pair<graphics::ShaderDataType, std::string>>& attribs,
    std::unordered_map<std::string, unsigned int> uniforms) {
    std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules;

    bool failed = false;
    for (const auto& [type, code] : sources) {
        VkShaderModuleCreateInfo createInfo{
          .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
          .codeSize = code.size() * sizeof(std::uint32_t),
          .pCode = code.data(),
        };

        VkShaderModule shaderModule;
        if (auto result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule); result != VK_SUCCESS) {
            PHENYL_LOGE(LOGGER, "Failed to create shader module");
            failed = true;
            break;
        }
        modules.emplace(type, shaderModule);
    }

    if (failed) {
        for (auto module : modules | std::views::values) {
            vkDestroyShaderModule(device, module, nullptr);
        }
        return nullptr;
    }

    return std::unique_ptr<VulkanShader>{new VulkanShader(device, std::move(modules), ShaderReflection{sources})};
}

VulkanShader::~VulkanShader () {
    for (auto module : m_modules | std::views::values) {
        vkDestroyShaderModule(m_device, module, nullptr);
    }
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanShader::getStageInfo () {
    return m_modules | std::views::transform([] (const auto& p) {
        auto [type, module] = p;

        return VkPipelineShaderStageCreateInfo{
          .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
          .stage = ConvertShaderType(type),
          .module = module,
          .pName = "main",
        };
    }) | std::ranges::to<std::vector>();
}

std::size_t VulkanShader::hash () const noexcept {
    return 1;
}

std::optional<unsigned int> VulkanShader::getAttribLocation (const std::string& attrib) const noexcept {
    auto* input = m_reflection.getAttrib(attrib);
    return input ? std::optional{input->location} : std::nullopt;
}

std::optional<unsigned int> VulkanShader::getUniformLocation (const std::string& uniform) const noexcept {
    auto* block = m_reflection.getUniformBlock(uniform);
    return block ? std::optional{block->location} : std::nullopt;
}

std::optional<unsigned int> VulkanShader::getSamplerLocation (const std::string& sampler) const noexcept {
    auto* reflectSampler = m_reflection.getSampler(sampler);
    return reflectSampler ? std::optional{reflectSampler->location} : std::nullopt;
}

std::optional<std::size_t> VulkanShader::getUniformOffset (const std::string& uniformBlock,
    const std::string& uniform) const noexcept {
    auto* block = m_reflection.getUniformBlock(uniformBlock);
    if (!block) {
        PHENYL_LOGD(LOGGER, "Failed to find uniform block \"{}\"", uniformBlock);
        return std::nullopt;
    }

    auto it = block->memberOffsets.find(uniform);
    PHENYL_LOGD_IF(it == block->memberOffsets.end(), LOGGER, "Failed to find uniform member \"{}::{}\"", uniformBlock,
        uniform);
    return it != block->memberOffsets.end() ? std::optional{it->second} : std::nullopt;
}

std::optional<std::size_t> VulkanShader::getUniformBlockSize (const std::string& uniformBlock) const noexcept {
    auto* block = m_reflection.getUniformBlock(uniformBlock);
    return block ? std::optional{block->size} : std::nullopt;
}

VulkanShaderManager::VulkanShaderManager (VkDevice device) : m_device{device} {}

std::shared_ptr<phenyl::graphics::Shader> VulkanShaderManager::load (core::AssetLoadContext& ctx) {
    PHENYL_ABORT("Unimplemented");
}

void VulkanShaderManager::selfRegister () {
    core::Assets::AddManager(this);
}

void VulkanShaderManager::loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader) {
    if (!shader) {
        PHENYL_LOGE(LOGGER, "Failed to load default shader \"{}\"", path);
        return;
    }

    PHENYL_LOGD(LOGGER, "Loaded default shader \"{}\"", path);
    m_defaultShaders.emplace_back(std::make_shared<graphics::Shader>(std::move(shader)));
    core::Assets::LoadVirtual(path, m_defaultShaders.back());
}

VulkanShaderManager::Builder VulkanShaderManager::builder () {
    return Builder{m_device, m_compiler};
}

VulkanShaderManager::Builder& VulkanShaderManager::Builder::withSource (graphics::ShaderSourceType type,
    const std::string& source) {
    auto code = m_compiler.compile(type, source);

    if (code) {
        m_sources.emplace(type, std::move(*code));
    }
    return *this;
}

VulkanShaderManager::Builder& VulkanShaderManager::Builder::withAttrib (graphics::ShaderDataType type,
    std::string attribName) {
    m_attribs.emplace_back(type, std::move(attribName));
    return *this;
}

VulkanShaderManager::Builder& VulkanShaderManager::Builder::withUniform (std::string attribName,
    unsigned int location) {
    m_uniformBindings.emplace(attribName, location);
    return *this;
}

std::unique_ptr<VulkanShader> VulkanShaderManager::Builder::build () {
    return VulkanShader::Make(m_device, m_sources, m_attribs, std::move(m_uniformBindings));
}
