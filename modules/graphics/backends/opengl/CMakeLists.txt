add_library(opengl_backend OBJECT src/opengl/glarray_texture.cpp src/opengl/glbuffer.cpp src/opengl/glframebuffer.cpp src/opengl/glimage_texture.cpp src/opengl/glpipeline.cpp src/opengl/glrenderer.cpp src/opengl/glsampler.cpp src/opengl/glshader.cpp src/opengl/gluniform_buffer.cpp)

set_property(TARGET opengl_backend PROPERTY CXX_STANDARD 20)

target_include_directories(opengl_backend PRIVATE src)
target_include_directories(opengl_backend PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

target_gen_embedded(opengl_backend
        resources/shaders/box_vertex.vert
        resources/shaders/box_fragment.frag
        resources/shaders/debug_vertex.vert
        resources/shaders/debug_fragment.frag
        resources/shaders/sprite_vertex.vert
        resources/shaders/sprite_fragment.frag
        resources/shaders/canvas_vertex.vert
        resources/shaders/canvas_fragment.frag
        resources/shaders/particle_vertex.vert
        resources/shaders/particle_fragment.frag
        resources/shaders/blinn_phong.vert
        resources/shaders/blinn_phong.frag
        resources/shaders/mesh_prepass.vert
        resources/shaders/postprocess.vert
        resources/shaders/noop_postprocess.frag
        resources/shaders/shadow_map.vert
)

target_link_libraries(opengl_backend PRIVATE GL GLEW::GLEW glfw_backend)
target_link_libraries(opengl_backend PUBLIC graphics_api)