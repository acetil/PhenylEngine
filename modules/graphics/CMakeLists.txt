add_library(graphics OBJECT src/graphics/opengl/glrenderer.cpp src/graphics/renderlayer/entity_layer.cpp src/graphics/renderlayer/canvas_layer.cpp src/graphics/image.cpp src/graphics/ui/debug_ui.cpp src/graphics/ui/ui_manager.cpp src/graphics/camera_2d.cpp src/stb/stb_image.cpp include/graphics/phenyl_graphics.h src/graphics/phenyl_graphics.cpp  include/graphics/ui/ui_defs.h src/graphics/renderlayer/debug_layer.cpp src/graphics/renderlayer/debug_layer.h
        include/graphics/components/2d/sprite.h
        include/graphics/ui/forward.h
        include/graphics/particles/particle_system_2d.h
        src/graphics/particles/particle_system_2d.cpp
        src/graphics/renderlayer/particle_layer.h
        src/graphics/renderlayer/particle_layer.cpp
        include/graphics/particles/particle_manager.h
        src/graphics/particles/particle_manager.cpp
        include/graphics/components/2d/particle_emitter.h
        src/graphics/particles/particle_emitter_2d.cpp
        include/graphics/graphics_properties.h
        include/graphics/detail/loggers.h
        src/graphics/detail/loggers.cpp
        include/graphics/plugins/ui_plugin.h
        src/graphics/plugins/ui_plugin.cpp
        include/graphics/plugins/graphics_plugin.h
        src/graphics/plugins/graphics_plugin.cpp
        include/graphics/plugins/particle_plugin.h
        src/graphics/plugins/particle_plugin.cpp
        include/graphics/plugins/sprite_2d_plugin.h
        src/graphics/plugins/sprite_2d_plugin.cpp
        include/graphics/plugins/graphics_2d_plugin.h
        include/graphics/viewport.h
        src/graphics/glfw/glfw_viewport.h
        src/graphics/glfw/glfw_viewport.cpp
        src/graphics/opengl/glbuffer.h
        src/graphics/opengl/glbuffer.cpp
        include/graphics/uniform_buffer.h
        src/graphics/opengl/gluniform_buffer.h
        src/graphics/opengl/gluniform_buffer.cpp
        src/graphics/opengl/glpipeline.h
        src/graphics/opengl/glpipeline.cpp
        include/graphics/abstract_render_layer.h
        src/graphics/opengl/glimage_texture.h
        src/graphics/opengl/glimage_texture.cpp
        src/graphics/opengl/glsampler.h
        src/graphics/opengl/glsampler.cpp
        src/graphics/opengl/glarray_texture.h
        src/graphics/opengl/glarray_texture.cpp
        include/graphics/font/font.h
        src/graphics/font/font.cpp
        include/graphics/font/glyph_atlas.h
        src/graphics/font/glyph_atlas.cpp
        src/graphics/font/font_manager.h
        src/graphics/font/font_manager.cpp
        include/graphics/font/glyph.h
        src/graphics/opengl/glshader.h
        src/graphics/opengl/glshader.cpp
        include/graphics/canvas/canvas.h
        include/graphics/canvas/canvas_style.h
        include/graphics/canvas/canvas_shapes.h
        src/graphics/canvas/canvas.cpp
        src/graphics/plugins/texture_manager.h
        src/graphics/plugins/texture_manager.cpp
        src/graphics/glfw/input/glfw_key_input.h
        src/graphics/glfw/input/glfw_key_input.cpp
        src/graphics/glfw/input/glfw_mouse_input.cpp
        src/graphics/glfw/input/glfw_mouse_input.h
        src/graphics/components/2d/sprite.cpp
        include/graphics/ui/widgets/widget.h
        src/graphics/ui/widgets/widget.cpp
        include/graphics/ui/widgets/label.h
        src/graphics/ui/widgets/label.cpp
        include/graphics/ui/widgets/layout.h
        src/graphics/ui/widgets/layout.cpp
        include/graphics/ui/widgets/root.h
        src/graphics/ui/widgets/root.cpp
        include/graphics/ui/widgets/container.h
        src/graphics/ui/widgets/container.cpp
        include/graphics/ui/widgets/event.h
        include/graphics/ui/widgets/button.h
        src/graphics/ui/widgets/button.cpp
        include/graphics/camera_3d.h
        src/graphics/camera_3d.cpp
        src/graphics/mesh/mesh.cpp
        src/graphics/plugins/mesh_manager.h
        src/graphics/plugins/mesh_manager.cpp
        src/graphics/filetypes/obj_file.h
        src/graphics/filetypes/obj_file.cpp
        include/graphics/plugins/mesh_3d_plugin.h
        src/graphics/renderlayer/mesh_layer.h
        include/graphics/components/3d/mesh_renderer.h
        src/graphics/renderlayer/mesh_layer.cpp
        include/graphics/plugins/graphics_3d_plugin.h
        src/graphics/plugins/mesh_3d_plugin.cpp
        src/graphics/components/3d/mesh_renderer.cpp
        include/graphics/material.h
        src/graphics/material.cpp
        src/graphics/plugins/material_manager.h
        src/graphics/plugins/material_manager.cpp
        src/graphics/plugins/material_instance_manager.h
        src/graphics/plugins/material_instance_manager.cpp
        include/graphics/components/3d/lighting.h
        src/graphics/components/3d/lighting.cpp
        include/graphics/framebuffer.h
        src/graphics/opengl/glframebuffer.h
        src/graphics/opengl/glframebuffer.cpp
)

cmake_policy(SET CMP0072 NEW) #FindOpenGL

#find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Freetype REQUIRED)
find_package(Harfbuzz REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(nlohmann_json REQUIRED)

set_property(TARGET graphics PROPERTY CXX_STANDARD 20)

target_gen_embedded(graphics
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
)

add_phenyl_resource(resources/fonts/NotoSerif-Regular.ttf resources/phenyl/fonts/noto-serif.ttf)
add_phenyl_resource(resources/fonts/OFL.txt resources/phenyl/fonts/OFL.txt)
add_phenyl_resource(resources/materials/blinn_phong.json resources/phenyl/materials/blinn_phong.json)

target_include_directories(graphics PUBLIC include)
target_include_directories(graphics PRIVATE src)
target_include_directories(graphics PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)

target_link_libraries(graphics PRIVATE logger)
target_link_libraries(graphics PRIVATE maths)
target_link_libraries(graphics PRIVATE util)
target_link_libraries(graphics PRIVATE harfbuzz::harfbuzz)
target_link_libraries(graphics PRIVATE Freetype::Freetype)
target_link_libraries(graphics PRIVATE glfw GL GLEW::GLEW)
target_link_libraries(graphics PRIVATE nlohmann_json)

target_link_libraries(graphics PUBLIC core)

add_library(graphics_interface INTERFACE)
target_include_directories(graphics_interface INTERFACE include)
target_link_libraries(graphics_interface INTERFACE runtime_interface)