cmake_minimum_required(VERSION 3.22)
project(phenyl_engine)
#cmake_policy(SET CMP0079 NEW)
enable_language(CXX)

add_compile_options("-D C_LOGGING")
set(DEBUG_COMPILE_FLAGS "-Werror=return-type")

set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb -fsanitize=address ${DEBUG_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(PythonInterp REQUIRED)
include(include.cmake)

add_subdirectory(modules/logger)
add_subdirectory(modules/util)
add_subdirectory(modules/common)
add_subdirectory(modules/component)
add_subdirectory(modules/physics)
add_subdirectory(modules/maths)
add_subdirectory(modules/graphics)
add_subdirectory(modules/engine)
add_subdirectory(modules/audio)
add_subdirectory(modules/runtime)

add_subdirectory(examples)

add_library(phenyl
        src/phenyl/application.cpp
        src/phenyl/plugins/app_plugin.cpp
        src/phenyl/plugins/default_2d_plugin.cpp
        src/phenyl/plugins/level_plugin.cpp
        src/phenyl/plugins/prefab_plugin.cpp
        src/phenyl/engine.cpp
        include/phenyl/properties.h)
target_include_directories(phenyl PUBLIC include)
target_include_directories(phenyl PRIVATE src)

set_property(TARGET phenyl PROPERTY CXX_STANDARD 20)

target_link_libraries(phenyl PUBLIC audio common component engine graphics logger maths physics runtime util)