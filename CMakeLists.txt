cmake_minimum_required(VERSION 3.29)
project(phenyl_engine)
#cmake_policy(SET CMP0079 NEW)
enable_language(CXX)

add_compile_options("-D C_LOGGING")
set(DEBUG_COMPILE_FLAGS "-Werror=return-type -ftime-trace")

set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -fvisibility=default -ggdb -fsanitize=address  ${DEBUG_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE True)

set(Python3_FIND_STRATEGY VERSION)
find_package(Python3 REQUIRED)

find_package(cpptrace REQUIRED)

set(PHENYL_RESOURCE_LIST "")
set(PHENYL_RESOURCE_TARGET_LIST "")

include(include.cmake)

add_subdirectory(modules/logger)
add_subdirectory(modules/util)
add_subdirectory(modules/core)
#add_subdirectory(modules/component)
add_subdirectory(modules/physics)
add_subdirectory(modules/maths)
add_subdirectory(modules/graphics)
add_subdirectory(modules/engine)
add_subdirectory(modules/audio)
add_subdirectory(modules/platform)
add_subdirectory(modules/ui)
#add_subdirectory(modules/runtime)

add_subdirectory(examples)

add_library(phenyl SHARED
        src/phenyl/application.cpp
        src/phenyl/plugins/app_plugin.cpp
        src/phenyl/plugins/default_2d_plugin.cpp
        src/phenyl/plugins/level_plugin.cpp
        src/phenyl/plugins/prefab_plugin.cpp
        src/phenyl/engine.cpp
        include/phenyl/properties.h
        include/phenyl/font.h
        include/phenyl/canvas.h
        include/phenyl/graphics/renderer.h
        include/phenyl/graphics/viewport.h
        include/phenyl/graphics/graphics.h
        include/phenyl/maths.h
        include/phenyl/serialization_fwd.h
        include/phenyl/ui/layout.h
        include/phenyl/ui/types.h
        include/phenyl/ui/container.h
        include/phenyl/ui/event.h
        include/phenyl/entrypoint.h
        include/phenyl/platform.h
        include/phenyl/plugins/default_3d_plugin.h
        include/phenyl/components/3D/global_transform.h
        include/phenyl/components/3D/mesh_renderer.h
        include/phenyl/graphics/mesh.h
        src/phenyl/plugins/default_3d_plugin.cpp
        include/phenyl/graphics/material.h
        include/phenyl/components/3D/lighting.h
        src/phenyl/engine_clock.h
        src/phenyl/engine_clock.cpp
        include/phenyl/ui/component.h
        include/phenyl/ui/atom.h)
target_include_directories(phenyl PUBLIC include)
target_include_directories(phenyl PRIVATE src)

set_property(TARGET phenyl PROPERTY CXX_STANDARD 20)

target_link_libraries(phenyl PUBLIC audio core engine graphics logger maths physics util platform glfw_backend opengl_backend vulkan_backend ui)
#target_link_libraries(phenyl PUBLIC cpptrace::cpptrace)
#target_link_libraries(phenyl PUBLIC $<COMPILE_ONLY:audio core engine graphics logger maths physics util>)
