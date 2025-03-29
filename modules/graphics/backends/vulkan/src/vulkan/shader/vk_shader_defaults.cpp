#include "vk_shader.h"

#include "resources/shaders/test.frag.h"
#include "resources/shaders/test.vert.h"
#include "resources/shaders/test_noop.frag.h"

using namespace phenyl::vulkan;

void VulkanShaderManager::loadDefaultShaders () {
    loadDefault("phenyl/shaders/box", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/debug", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/sprite", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/canvas", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/particle", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/blinn_phong", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/shadow_map", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/mesh_prepass", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());
    loadDefault("phenyl/shaders/postprocess/noop", builder()
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_NOOP_FRAG)
        .build());

    loadDefault("phenyl/shaders/test", builder()
        .withSource(graphics::ShaderSourceType::VERTEX, EMBED_TEST_VERT)
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_FRAG)
        .build());
}