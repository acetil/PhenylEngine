add_library(vulkan_backend OBJECT src/vulkan/vk_renderer.cpp
        src/vulkan/init/vk_device.cpp
        src/vulkan/init/vk_swap_chain.cpp
        src/vulkan/vk_storage_buffer.cpp
        src/vulkan/vk_array_texture.cpp
        src/vulkan/vk_framebuffer.cpp
        src/vulkan/vk_image_texture.cpp
        src/vulkan/vk_pipeline.cpp
        src/vulkan/shader/vk_shader.cpp
        src/vulkan/vk_uniform_buffer.cpp
        src/vulkan/shader/vk_compiler.h
        src/vulkan/shader/vk_compiler.cpp
        src/vulkan/shader/vk_shader_defaults.cpp
        src/vulkan/vk_command_buffer.h
        src/vulkan/vk_command_buffer.cpp
        src/vulkan/vk_sync.h
        src/vulkan/vk_sync.cpp
        src/vulkan/vk_frame.h
        src/vulkan/vk_frame.cpp
        src/vulkan/memory/vk_buffer.h
        src/vulkan/memory/vma.cpp
        src/vulkan/memory/vk_buffer.cpp
        src/vulkan/vk_descriptors.h
        src/vulkan/vk_descriptors.cpp
        src/vulkan/shader/vk_shader_reflection.h
        src/vulkan/shader/vk_shader_reflection.cpp
        src/vulkan/init/vk_resources.h
        src/vulkan/init/vk_resources.cpp
        src/vulkan/texture/vk_image.h
        src/vulkan/texture/vk_image.cpp
        src/vulkan/memory/vk_transfer_manager.h
        src/vulkan/memory/vk_transfer_manager.cpp
)

set_property(TARGET vulkan_backend PROPERTY CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(VulkanUtilityLibraries REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_cpp CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)

target_include_directories(vulkan_backend PRIVATE src)
target_include_directories(vulkan_backend PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)

target_gen_embedded(vulkan_backend
        resources/shaders/blinn_phong.frag
        resources/shaders/blinn_phong.vert
        resources/shaders/box_fragment.frag
        resources/shaders/box_vertex.vert
        resources/shaders/canvas_fragment.frag
        resources/shaders/canvas_vertex.vert
        resources/shaders/debug_fragment.frag
        resources/shaders/debug_vertex.vert
        resources/shaders/mesh_prepass.vert
        resources/shaders/noop_postprocess.frag
        resources/shaders/particle_fragment.frag
        resources/shaders/particle_vertex.vert
        resources/shaders/postprocess.vert
        resources/shaders/shadow_map.vert
        resources/shaders/sprite_fragment.frag
        resources/shaders/sprite_vertex.vert
        resources/shaders/test.frag
        resources/shaders/test.vert
)

target_link_libraries(vulkan_backend PRIVATE Vulkan::Vulkan Vulkan::UtilityHeaders GPUOpen::VulkanMemoryAllocator)
target_link_libraries(vulkan_backend PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)
target_link_libraries(vulkan_backend PRIVATE spirv-cross-cpp spirv-cross-reflect)
target_link_libraries(vulkan_backend PRIVATE glfw_backend logger core)
target_link_libraries(vulkan_backend PUBLIC graphics_api)
