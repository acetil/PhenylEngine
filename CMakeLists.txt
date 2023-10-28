cmake_minimum_required(VERSION 3.26)
project(phenyl_engine)
#cmake_policy(SET CMP0079 NEW)
enable_language(CXX)

add_compile_options("-D C_LOGGING -stdlib=libc++")
set(DEBUG_COMPILE_FLAGS "-Werror=return-type")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")

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

add_subdirectory(examples)

add_library(phenyl src/phenyl/application.cppm
        src/phenyl/asset.cppm
        src/phenyl/audio.cppm
        src/phenyl/component.cppm
        src/phenyl/engine.cppm
        src/phenyl/input.cppm
        src/phenyl/level.cppm
        src/phenyl/prefab.cppm
        src/phenyl/transform_2d.cppm
        src/phenyl/components/audio_player.cppm
        src/phenyl/components/2D/global_transform.cppm
        src/phenyl/components/2D/particle_emitter.cppm
        src/phenyl/components/2D/sprite.cppm
        src/phenyl/components/2D/physics/rigid_body.cppm
        src/phenyl/components/2D/physics/collider.cppm
        src/phenyl/components/2D/physics/colliders/box_collider.cppm
        src/phenyl/signals/lifecycle.cppm
        src/phenyl/signals/physics.cppm
        src/phenyl/ui/button.cppm
        src/phenyl/ui/flex_box.cppm
        src/phenyl/ui/label.cppm
        src/phenyl/ui/theme.cppm
        src/phenyl/signals/signals.cppm
        src/phenyl/ui/ui.cppm
        src/phenyl/components/components.cppm
        src/phenyl/components/2D/components2d.cppm
        src/phenyl/components/2D/physics/physics.cppm
        src/phenyl/phenyl.cppm
)
target_include_directories(phenyl PUBLIC include)
target_link_libraries(phenyl PUBLIC audio common component engine graphics logger maths physics util)

target_sources(phenyl PUBLIC FILE_SET CXX_MODULES FILES
        src/phenyl/application.cppm
        src/phenyl/asset.cppm
        src/phenyl/audio.cppm
        src/phenyl/component.cppm
        src/phenyl/engine.cppm
        src/phenyl/input.cppm
        src/phenyl/level.cppm
        src/phenyl/prefab.cppm
        src/phenyl/transform_2d.cppm
        src/phenyl/components/audio_player.cppm
        src/phenyl/components/2D/global_transform.cppm
        src/phenyl/components/2D/particle_emitter.cppm
        src/phenyl/components/2D/sprite.cppm
        src/phenyl/components/2D/physics/rigid_body.cppm
        src/phenyl/components/2D/physics/collider.cppm
        src/phenyl/components/2D/physics/colliders/box_collider.cppm
        src/phenyl/signals/lifecycle.cppm
        src/phenyl/signals/physics.cppm
        src/phenyl/ui/button.cppm
        src/phenyl/ui/flex_box.cppm
        src/phenyl/ui/label.cppm
        src/phenyl/ui/theme.cppm
        src/phenyl/signals/signals.cppm
        src/phenyl/ui/ui.cppm
        src/phenyl/components/components.cppm
        src/phenyl/components/2D/components2d.cppm
        src/phenyl/components/2D/physics/physics.cppm
        src/phenyl/phenyl.cppm
)