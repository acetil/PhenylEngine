cmake_minimum_required(VERSION 3.22)
project(action-game)
#cmake_policy(SET CMP0079 NEW)
enable_language(CXX)

add_compile_options("-D C_LOGGING")
set(DEBUG_COMPILE_FLAGS "-Werror=return-type")

set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb -fsanitize=address ${DEBUG_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS "${DEBUG_COMPILE_FLAGS}")
#set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb -Wreturn-type")
#set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/main.cpp src/util/debug_console.cpp
        src/game/entity/bullet.h
        src/game/entity/player.h
        src/game/entity/serializers.h
        src/game/entity/bullet.cpp
        src/game/entity/player.cpp
        src/game/test_app.h
        src/game/test_app.cpp)

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
add_subdirectory(modules/component)
add_subdirectory(modules/physics)
add_subdirectory(modules/maths)
add_subdirectory(modules/graphics)
add_subdirectory(modules/engine)

target_link_libraries(${PROJECT_NAME} PRIVATE logger util component physics maths graphics engine common)

