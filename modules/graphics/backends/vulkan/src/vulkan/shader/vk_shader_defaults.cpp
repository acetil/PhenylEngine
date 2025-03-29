#include "vk_shader.h"

#include "resources/shaders/test.frag.h"
#include "resources/shaders/test.vert.h"

using namespace phenyl::vulkan;

void VulkanShaderManager::loadDefaultShaders () {
    loadDefault("phenyl/shaders/box", nullptr);
    loadDefault("phenyl/shaders/debug", nullptr);
    loadDefault("phenyl/shaders/sprite", nullptr);
    loadDefault("phenyl/shaders/canvas", nullptr);
    loadDefault("phenyl/shaders/particle", nullptr);
    loadDefault("phenyl/shaders/blinn_phong", nullptr);
    loadDefault("phenyl/shaders/shadow_map", nullptr);
    loadDefault("phenyl/shaders/mesh_prepass", nullptr);
    loadDefault("phenyl/shaders/postprocess/noop", nullptr);

    loadDefault("phenyl/shaders/test", builder()
        .withSource(graphics::ShaderSourceType::VERTEX, EMBED_TEST_VERT)
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_FRAG)
        .build());
}