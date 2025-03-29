add_library(vulkan_backend OBJECT src/vulkan/vk_renderer.cpp
        src/vulkan/vk_device.cpp
        src/vulkan/vk_swap_chain.cpp)

set_property(TARGET vulkan_backend PROPERTY CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(VulkanUtilityLibraries REQUIRED)

target_include_directories(vulkan_backend PRIVATE src)

target_link_libraries(vulkan_backend PRIVATE Vulkan::Vulkan Vulkan::UtilityHeaders)
target_link_libraries(vulkan_backend PRIVATE glfw_backend logger)
target_link_libraries(vulkan_backend PUBLIC graphics_api)