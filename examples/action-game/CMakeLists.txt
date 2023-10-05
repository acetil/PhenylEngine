add_executable(action-game src/main.cpp src/util/debug_console.cpp
        src/game/entity/bullet.h
        src/game/entity/player.h
        src/game/entity/serializers.h
        src/game/entity/bullet.cpp
        src/game/entity/player.cpp
        src/game/test_app.h
        src/game/test_app.cpp)

set_property(TARGET action-game PROPERTY CXX_STANDARD 20)
target_compile_options(action-game PRIVATE "-g")

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

target_include_directories(action-game PRIVATE src)
target_link_libraries(action-game PRIVATE phenyl)