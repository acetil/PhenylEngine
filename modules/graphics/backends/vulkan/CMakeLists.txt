add_library(vulkan_backend OBJECT src/vulkan/vk_renderer.cpp
        src/vulkan/init/vk_device.cpp
        src/vulkan/init/vk_swap_chain.cpp
        src/vulkan/vk_buffer.cpp
        src/vulkan/vk_array_texture.cpp
        src/vulkan/vk_framebuffer.cpp
        src/vulkan/vk_image_texture.cpp
        src/vulkan/vk_pipeline.cpp
        src/vulkan/vk_shader.cpp
        src/vulkan/vk_uniform_buffer.cpp)

set_property(TARGET vulkan_backend PROPERTY CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(VulkanUtilityLibraries REQUIRED)

target_include_directories(vulkan_backend PRIVATE src)

target_link_libraries(vulkan_backend PRIVATE Vulkan::Vulkan Vulkan::UtilityHeaders)
target_link_libraries(vulkan_backend PRIVATE glfw_backend logger core)
target_link_libraries(vulkan_backend PUBLIC graphics_api)