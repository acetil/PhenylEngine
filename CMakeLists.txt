cmake_minimum_required(VERSION 3.0)
project(action-game)
cmake_policy(SET CMP0072 NEW) #FindOpenGL
enable_language(CXX)
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp)

set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG_LOG -ggdb")
link_directories(${CMAKE_SOURCE_DIR}/lib)

include_directories(${CMAKE_BINARY_DIR}/src)


add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set (CMAKE_MODULE_PATH $(CMAKE_MODULE_PATH) "${CMAKE_SOURCE_DIR}/cmake/")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(harfbuzz REQUIRED)

get_target_property(harfbuzz_include_dir harfbuzz::harfbuzz INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(harfbuzz_icu_include_dir harfbuzz::icu INTERFACE_INCLUDE_DIRECTORIES)
if (WIN32)
set(LIBS glfw3 opengl32 GLEW::GLEW FreeType harfbuzz )
elseif (UNIX)
set(LIBS glfw GL GLEW::GLEW freetype harfbuzz)
endif(WIN32)
#message(harfbuzz)
include_directories(
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
        "${harfbuzz_include_dir}"
        #"${harfbuzz_icu_include_dir}"
        "${FREETYPE_INCLUDE_DIRS}"
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
