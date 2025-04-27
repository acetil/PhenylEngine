#include "vk_shader.h"

#include "resources/shaders/blinn_phong.frag.h"
#include "resources/shaders/blinn_phong.vert.h"
#include "resources/shaders/box_fragment.frag.h"
#include "resources/shaders/box_vertex.vert.h"
#include "resources/shaders/canvas_fragment.frag.h"
#include "resources/shaders/canvas_vertex.vert.h"
#include "resources/shaders/debug_fragment.frag.h"
#include "resources/shaders/debug_vertex.vert.h"
#include "resources/shaders/mesh_prepass.vert.h"
#include "resources/shaders/noop_postprocess.frag.h"
#include "resources/shaders/particle_fragment.frag.h"
#include "resources/shaders/particle_vertex.vert.h"
#include "resources/shaders/postprocess.vert.h"
#include "resources/shaders/shadow_map.vert.h"
#include "resources/shaders/sprite_fragment.frag.h"
#include "resources/shaders/sprite_vertex.vert.h"

#include "resources/shaders/test.frag.h"
#include "resources/shaders/test.vert.h"

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

void VulkanShaderManager::loadDefaultShaders () {
    loadDefault("phenyl/shaders/box", builder()
        .withSource(graphics::ShaderSourceType::VERTEX, EMBED_BOX_VERTEX_VERT)
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_BOX_FRAGMENT_FRAG)
        .withAttrib(ShaderDataType::VEC2F, "pos")
        .withAttrib(ShaderDataType::VEC2F, "rectPosIn")
        .withAttrib(ShaderDataType::VEC4F, "borderColourIn")
        .withAttrib(ShaderDataType::VEC4F, "bgColourIn")
        .withAttrib(ShaderDataType::VEC4F, "boxDetailIn")
        .withUniform("Uniform", 0)
        .build());

    loadDefault("phenyl/shaders/debug", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_DEBUG_VERTEX_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_DEBUG_FRAGMENT_FRAG)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::VEC4F, "colourOut")
        .withUniform("Uniform", 0)
        .build());

    loadDefault("phenyl/shaders/sprite", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_SPRITE_VERTEX_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_SPRITE_FRAGMENT_FRAG)
        .withAttrib(ShaderDataType::VEC2F, "position")
        .withAttrib(ShaderDataType::VEC2F, "uvOut")
        .withUniform("Camera", 0)
        .build());

    loadDefault("phenyl/shaders/canvas", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_CANVAS_VERTEX_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_CANVAS_FRAGMENT_FRAG)
        .withAttrib(ShaderDataType::VEC2F, "pos")
        .withAttrib(ShaderDataType::VEC3F, "uvOut")
        .withAttrib(ShaderDataType::VEC4F, "colorOut")
        .withUniform("Uniform", 0)
        .build());

    loadDefault("phenyl/shaders/particle", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_PARTICLE_VERTEX_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_PARTICLE_FRAGMENT_FRAG)
        .withAttrib(ShaderDataType::VEC2F, "pos")
        .withAttrib(ShaderDataType::VEC4F, "colourIn")
        .withUniform("Camera", 0)
        .build());

    loadDefault("phenyl/shaders/blinn_phong", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_BLINN_PHONG_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_BLINN_PHONG_FRAG)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::VEC3F, "normal")
        .withAttrib(ShaderDataType::VEC2F, "texcoord_0")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniform("GlobalUniform", 0)
        .withUniform("BPLightUniform", 1)
        .withUniform("Material", 2)
        .build());

    loadDefault("phenyl/shaders/shadow_map", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_SHADOW_MAP_VERT)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniform("BPLightUniform", 0)
        .build());

    loadDefault("phenyl/shaders/mesh_prepass", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_MESH_PREPASS_VERT)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniform("GlobalUniform", 0)
        .build());

    loadDefault("phenyl/shaders/postprocess/noop", builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_POSTPROCESS_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_NOOP_POSTPROCESS_FRAG)
        .build());

    loadDefault("phenyl/shaders/test", builder()
        .withSource(graphics::ShaderSourceType::VERTEX, EMBED_TEST_VERT)
        .withSource(graphics::ShaderSourceType::FRAGMENT, EMBED_TEST_FRAG)
        .build());
}