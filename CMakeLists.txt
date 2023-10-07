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

add_subdirectory(examples)

add_library(phenyl INTERFACE include/phenyl/phenyl.h
        include/phenyl/application.h
        include/phenyl/engine.h
        include/phenyl/input.h
        include/phenyl/component.h
        include/phenyl/components/2D/particle_emitter.h)
target_include_directories(phenyl INTERFACE include)
target_link_libraries(phenyl INTERFACE common component engine graphics logger maths physics util)