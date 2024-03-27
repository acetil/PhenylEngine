add_library(graphics src/graphics/graphics.cpp src/graphics/font/font.cpp src/graphics/font/font_face.cpp src/graphics/font/font_manager.cpp src/graphics/font/glyph_atlas.cpp src/graphics/opengl/glrenderer.cpp src/graphics/renderlayer/entity_layer.cpp src/graphics/renderlayer/ui_layer.cpp src/graphics/textures/build_atlas.cpp src/graphics/textures/image.cpp src/graphics/ui/debug_ui.cpp src/graphics/ui/ui_manager.cpp src/graphics/camera.cpp src/graphics/graphics.cpp src/graphics/graphics_init.cpp src/stb/stb_image.cpp include/graphics/phenyl_graphics.h src/graphics/phenyl_graphics.cpp include/graphics/shaders/shaders.h src/graphics/opengl/glshader.h src/graphics/shaders/renderer_shader.h src/graphics/opengl/glshader.cpp src/graphics/shaders/shaders.cpp include/graphics/font/rendered_text.h src/graphics/font/rendered_text.cpp include/graphics/ui/ui_rect.h include/graphics/ui/nodes/ui_node.h include/graphics/ui/nodes/ui_anchor.h include/graphics/ui/nodes/ui_button.h src/graphics/ui/nodes/ui_button.cpp include/graphics/ui/nodes/ui_root.h src/graphics/ui/nodes/ui_root.cpp src/graphics/glfw/input/glfw_key_input.h src/graphics/glfw/input/glfw_key_input.cpp src/graphics/glfw/input/glfw_mouse_input.h src/graphics/glfw/input/glfw_mouse_input.cpp include/graphics/ui/nodes/ui_flexbox.h include/graphics/ui/ui_defs.h src/graphics/ui/nodes/ui_flexbox.cpp src/graphics/ui/ui_defs.cpp include/graphics/ui/nodes/ui_label.h src/graphics/ui/nodes/ui_label.cpp include/graphics/ui/themes/theme.h src/graphics/ui/themes/theme.cpp include/graphics/ui/themes/theme_class.h include/graphics/ui/themes/forward.h src/graphics/ui/themes/theme_class.cpp include/graphics/ui/themes/theme_properties.h src/graphics/ui/themes/theme_properties.cpp include/graphics/ui/ui_data.h src/graphics/ui/ui_data.cpp src/graphics/glfw/input/glfw_input.h src/graphics/glfw/input/glfw_input.cpp include/graphics/ui/components/ui_component.h include/graphics/ui/nodes/forward.h include/graphics/ui/components/ui_button.h src/graphics/ui/components/ui_button.cpp include/graphics/ui/components/ui_flexbox.h src/graphics/ui/components/ui_flexbox.cpp include/graphics/ui/components/ui_label.h src/graphics/ui/components/ui_label.cpp include/graphics/ui/nodes/ui_container.h src/graphics/ui/nodes/ui_node.cpp src/graphics/ui/nodes/ui_container.cpp src/graphics/renderlayer/debug_layer.cpp src/graphics/renderlayer/debug_layer.h include/graphics/debug_graphics.h
        include/graphics/textures/texture_atlas.h
        src/graphics/textures/texture_atlas_new.cpp
        include/graphics/textures/sprite_atlas.h
        include/graphics/components/2d/sprite.h
        include/graphics/components/2d/sprite_serialization.h
        src/graphics/textures/sprite_atlas.cpp
        include/graphics/ui/forward.h
        include/graphics/particles/particle_system_2d.h
        src/graphics/particles/particle_system_2d.cpp
        src/graphics/renderlayer/particle_layer.h
        src/graphics/renderlayer/particle_layer.cpp
        include/graphics/particles/particle_manager.h
        src/graphics/particles/particle_manager.cpp
        include/graphics/components/2d/particle_emitter.h
        src/graphics/particles/particle_emitter_2d.cpp
        include/graphics/components/2d/particle_emitter_serialization.h
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
        include/graphics/renderers/buffer.h
        src/graphics/opengl/glbuffer.h
        src/graphics/opengl/glbuffer.cpp
        include/graphics/renderers/pipeline.h
        include/graphics/renderers/uniform_buffer.h
        src/graphics/opengl/gluniform_buffer.h
        src/graphics/opengl/gluniform_buffer.cpp
        src/graphics/opengl/glpipeline.h
        src/graphics/opengl/glpipeline.cpp
        include/graphics/abstract_render_layer.h
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
        resources/shaders/text_vertex.vert
        resources/shaders/text_fragment.frag
        resources/shaders/particle_vertex.vert
        resources/shaders/particle_fragment.frag
        resources/themes/default_theme.json)

target_include_directories(graphics PUBLIC include)
target_include_directories(graphics PRIVATE src)
target_include_directories(graphics PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)

target_link_libraries(graphics PRIVATE logger)
target_link_libraries(graphics PRIVATE component)
target_link_libraries(graphics PRIVATE maths)
target_link_libraries(graphics PRIVATE util)
target_link_libraries(graphics PRIVATE engine)
target_link_libraries(graphics PRIVATE common)
#target_link_libraries(graphics PRIVATE entity_events)
target_link_libraries(graphics PRIVATE harfbuzz::harfbuzz)
target_link_libraries(graphics PRIVATE Freetype::Freetype)
target_link_libraries(graphics PRIVATE glfw GL GLEW::GLEW)
target_link_libraries(graphics PRIVATE nlohmann_json)
target_link_libraries(graphics PUBLIC runtime)