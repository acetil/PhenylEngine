add_library(glfw_backend OBJECT src/glfw/glfw_viewport.cpp src/glfw/input/glfw_key_input.cpp src/glfw/input/glfw_mouse_input.cpp src/glfw/glfw_viewport.h)

set_property(TARGET glfw_backend PROPERTY CXX_STANDARD 20)

cmake_policy(SET CMP0072 NEW) #FindOpenGL

find_package(glfw3 REQUIRED)

target_include_directories(glfw_backend PUBLIC src)
target_include_directories(glfw_backend PRIVATE src)

target_link_libraries(glfw_backend PRIVATE glfw)
target_link_libraries(glfw_backend PUBLIC graphics_api)