#include "vk_compiler.h"

#include "vulkan/vulkan_headers.h"

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_COMPILER", detail::VULKAN_LOGGER};

constexpr glslang::EshTargetClientVersion DEFAULT_CLIENT_VERSION = glslang::EShTargetVulkan_1_3;

static EShLanguage GetEshLang (phenyl::graphics::ShaderSourceType shaderType) {
    switch (shaderType) {
    case phenyl::graphics::ShaderSourceType::FRAGMENT:
        return EShLangFragment;
    case phenyl::graphics::ShaderSourceType::VERTEX:
        return EShLangVertex;
    }

    PHENYL_ABORT("Unknown shader type: {}", static_cast<std::uint32_t>(shaderType));
}

VulkanShaderCompiler::VulkanShaderCompiler () {
    glslang::InitializeProcess();
}

VulkanShaderCompiler::~VulkanShaderCompiler () {
    glslang::FinalizeProcess();
}

std::optional<std::vector<std::uint32_t>> VulkanShaderCompiler::compile (graphics::ShaderSourceType shaderType,
    const std::string& glslCode) const {
    auto lang = GetEshLang(shaderType);
    auto messageFilter = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault | EShMsgDebugInfo);

    glslang::TShader shader{lang};

    auto* codeCstr = glslCode.c_str();
    shader.setStrings(&codeCstr, 1);

    shader.setEnvInput(glslang::EShSourceGlsl, lang, glslang::EShClientVulkan, 450);
    shader.setEnvClient(glslang::EShClientVulkan, DEFAULT_CLIENT_VERSION);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_6);

    auto* resources = GetDefaultResources();

    if (!shader.parse(resources, DEFAULT_CLIENT_VERSION, true, messageFilter)) {
        PHENYL_LOGW(LOGGER, "Shader parse error in {} shader", shaderType);
        PHENYL_LOGW(LOGGER, "glslang info log: {}", shader.getInfoLog());
        // PHENYL_LOGW(LOGGER, "glslang debug log: {}", shader.getInfoDebugLog());

        return std::nullopt;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messageFilter)) {
        PHENYL_LOGW(LOGGER, "Shader link error in {} shader", shaderType);
        PHENYL_LOGW(LOGGER, "glslang info log: {}", program.getInfoLog());
        // PHENYL_LOGW(LOGGER, "glslang debug log: {}", program.getInfoDebugLog());

        return std::nullopt;
    }

    if (!program.mapIO()) {
        PHENYL_LOGW(LOGGER, "Shader map IO error in {} shader", shaderType);
        PHENYL_LOGW(LOGGER, "glslang info log: {}", program.getInfoLog());
        // PHENYL_LOGW(LOGGER, "glslang debug log: {}", program.getInfoDebugLog());

        return std::nullopt;
    }

    glslang::SpvOptions spvOptions{
      .generateDebugInfo = true,
      .stripDebugInfo = false,
      .disableOptimizer = true,
      .validate = true,
      .emitNonSemanticShaderDebugInfo = true,
    };

    spv::SpvBuildLogger spvLogger;
    std::vector<std::uint32_t> spirvBytecode;
    glslang::GlslangToSpv(*program.getIntermediate(lang), spirvBytecode, &spvLogger, &spvOptions);

    if (!spvLogger.getAllMessages().empty()) {
        PHENYL_LOGD(LOGGER, "SPIRV conversion log for {} shader: {}", shaderType, spvLogger.getAllMessages());
    }

    return spirvBytecode;
}
