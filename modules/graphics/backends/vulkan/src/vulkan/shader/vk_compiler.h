#pragma once

#include "graphics/backend/shader.h"

#include <iosfwd>
#include <memory>
#include <string>

namespace phenyl::vulkan {
class VulkanShaderCompiler {
public:
    VulkanShaderCompiler ();
    ~VulkanShaderCompiler ();

    std::optional<std::vector<std::uint32_t>> compile (graphics::ShaderSourceType shaderType,
        const std::string& glslCode) const;
};
} // namespace phenyl::vulkan
