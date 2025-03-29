#include "vk_shader.h"

#include <ranges>

#include "core/assets/assets.h"

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

VulkanShader::VulkanShader (VkDevice device, std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules) : device{device}, modules{std::move(modules)} {}

std::unique_ptr<VulkanShader> VulkanShader::Make (VkDevice device, const std::unordered_map<graphics::ShaderSourceType, std::vector<std::uint32_t>>& sources) {
    std::unordered_map<graphics::ShaderSourceType, VkShaderModule> modules;

    bool failed = false;
    for (const auto& [type, code] : sources) {
        VkShaderModuleCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size() * sizeof(std::uint32_t),
            .pCode = code.data()
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

    return std::unique_ptr<VulkanShader>{new VulkanShader(device, std::move(modules))};
}

VulkanShader::~VulkanShader () {
    for (auto module : modules | std::views::values) {
        vkDestroyShaderModule(device, module, nullptr);
    }
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanShader::getStageInfo () {
    return modules
        | std::views::transform([] (const auto& p) {
                auto [type, module] = p;

                return VkPipelineShaderStageCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = ConvertShaderType(type),
                    .module = module,
                    .pName = "main"
                };
            })
        | std::ranges::to<std::vector>();
}

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

VulkanShaderManager::VulkanShaderManager (VkDevice device) : device{device} {}

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

void VulkanShaderManager::loadDefault (const std::string& path, std::unique_ptr<VulkanShader> shader) {
    if (!shader) {
        PHENYL_LOGE(LOGGER, "Failed to load default shader \"{}\"", path);
        return;
    }

    PHENYL_LOGD(LOGGER, "Loaded default shader \"{}\"", path);
    defaultShaders.emplace_back(core::Assets::LoadVirtual(path, graphics::Shader{std::move(shader)}));
}

VulkanShaderManager::Builder VulkanShaderManager::builder () {
    return Builder{device, compiler};
}

VulkanShaderManager::Builder& VulkanShaderManager::Builder::withSource (graphics::ShaderSourceType type,
                                                                        const std::string& source) {
    auto code = compiler.compile(type, source);

    if (code) {
        shaderSources.emplace(type, std::move(*code));
    }
    return *this;
}

std::unique_ptr<VulkanShader> VulkanShaderManager::Builder::build () {
    return VulkanShader::Make(device, shaderSources);
}
