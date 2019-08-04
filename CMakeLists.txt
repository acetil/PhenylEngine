cmake_minimum_required(VERSION 3.0)
project(action-game)
enable_language(CXX)
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp)

link_directories(${CMAKE_SOURCE_DIR}/lib)

configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set (CMAKE_MODULE_PATH $(CMAKE_MODULE_PATH) "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)

set(LIBS glfw3 opengl32 GLEW::GLEW)

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
)
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
target_link_libraries(${PROJECT_NAME} ${LIBS})
