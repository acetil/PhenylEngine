cmake_minimum_required(VERSION 3.22)
project(action-game)
#cmake_policy(SET CMP0079 NEW)
enable_language(CXX)

add_compile_options("-D C_LOGGING")
#set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb")
#set(CMAKE_CXX_STANDARD 20)

#add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} modules/graphics/src/graphics/ui/ui_manager.cpp modules/graphics/include/graphics/renderlayer/ui_layer.h modules/graphics/src/graphics/renderlayer/ui_layer.cpp modules/component/include/component/view/debug_game_view.h modules/graphics/include/graphics/ui/debug_ui.h modules/graphics/src/graphics/ui/debug_ui.cpp src/util/debug_console.h src/util/debug_console.cpp src/event/events/debug/profiler_change.h src/event/events/map_load.h src/event/events/debug/reload_map.h src/game/entity/controller/wall_controller.h src/game/entity/controller/wall_controller.cpp modules/engine/src/engine/entity/basic_entity.h modules/engine/src/engine/game_camera.h modules/engine/src/engine/game_camera.cpp modules/component/include/component/serialisable_component.h modules/component/include/component/component_serialisation.h src/event/events/debug/dump_map.h modules/component/src/component/main_component.cpp modules/maths/src/maths_headers.cpp src/event/events/map_load_request.h)
add_executable(${PROJECT_NAME} src/main.cpp src/game/entity/controller/bullet_controller.cpp src/game/entity/controller/player_controller.cpp src/game/entity/controller/wall_controller.cpp src/game/gameloop.cpp src/util/debug_console.cpp src/game/default_input.h src/game/default_input.cpp)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(${PROJECT_NAME} PRIVATE src)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

file(GLOB_RECURSE resource_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "resources/*")
foreach(resource_file ${resource_files})
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${resource_file}"
        COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${resource_file}" "${CMAKE_CURRENT_BINARY_DIR}/${resource_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${resource_file}"
    )
    list(APPEND resource_files_dest "${resource_file}")
endforeach()
add_custom_target(CopyResources ALL DEPENDS ${resource_files_dest})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

add_subdirectory(modules/logger)
add_subdirectory(modules/util)
add_subdirectory(modules/common)
add_subdirectory(modules/eventbus)
add_subdirectory(modules/component)
add_subdirectory(modules/physics)
add_subdirectory(modules/maths)
add_subdirectory(modules/graphics)
#add_subdirectory(modules/entity_events)
add_subdirectory(modules/engine)
#add_subdirectory(modules/common)

target_link_libraries(${PROJECT_NAME} PRIVATE logger util eventbus component physics maths graphics engine common)

