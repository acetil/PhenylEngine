add_library(graphics src/graphics/graphics.cpp src/graphics/font/font.cpp src/graphics/font/font_face.cpp src/graphics/font/font_manager.cpp src/graphics/font/glyph_atlas.cpp src/graphics/renderers/glcallbacks.cpp src/graphics/renderers/glrenderer.cpp src/graphics/renderers/window_callbacks.cpp src/graphics/renderlayer/entity_layer.cpp src/graphics/renderlayer/graphics_layer.cpp src/graphics/renderlayer/map_layer.cpp src/graphics/renderlayer/ui_layer.cpp src/graphics/shaders/shaders.cpp src/graphics/textures/build_atlas.cpp src/graphics/textures/image.cpp src/graphics/textures/texture_atlas.cpp src/graphics/ui/debug_ui.cpp src/graphics/ui/ui_manager.cpp src/graphics/buffer.cpp src/graphics/camera.cpp src/graphics/graphics.cpp src/graphics/graphics_init.cpp src/graphics/graphics_update.cpp src/stb/stb_image.cpp include/graphics/phenyl_graphics.h src/graphics/phenyl_graphics.cpp)

cmake_policy(SET CMP0072 NEW) #FindOpenGL

#find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Freetype REQUIRED)
find_package(Harfbuzz REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set_property(TARGET graphics PROPERTY CXX_STANDARD 20)

target_include_directories(graphics PUBLIC include)
target_include_directories(graphics PRIVATE src)

target_link_libraries(graphics PRIVATE logger)
target_link_libraries(graphics PRIVATE eventbus)
target_link_libraries(graphics PRIVATE component)
target_link_libraries(graphics PRIVATE maths)
target_link_libraries(graphics PRIVATE util)
target_link_libraries(graphics PRIVATE engine)
target_link_libraries(graphics PRIVATE entity_events)
target_link_libraries(graphics PRIVATE harfbuzz::harfbuzz)
target_link_libraries(graphics PRIVATE Freetype::Freetype)
target_link_libraries(graphics PRIVATE glfw GL GLEW::GLEW)