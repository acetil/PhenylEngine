cmake_minimum_required(VERSION 3.22)

function(target_gen_embedded embed_target)
    set(EMBED_EXEC ${PROJECT_SOURCE_DIR}/gen_embed.py)
    foreach (EMBED_FILE ${ARGN})
        cmake_path(GET EMBED_FILE PARENT_PATH EMBED_DIR)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_DIR})

        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_FILE}.h
            COMMAND
                ${Python3_EXECUTABLE} ${EMBED_EXEC} ${EMBED_FILE} ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_FILE}.h
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS
                ${EMBED_EXEC} ${EMBED_FILE}
            VERBATIM
        )
        target_sources(${embed_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_FILE}.h)
    endforeach()
    target_include_directories(${embed_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)
endfunction()

function(target_copy_resources resource_target)
    foreach (RES_FILE ${ARGN})
        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE}
            COMMAND
                cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${RES_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${RES_FILE}"
            DEPENDS
                ${CMAKE_CURRENT_SOURCE_DIR}/${RES_FILE}
            VERBATIM
        )
    endforeach()

    list(TRANSFORM ARGN PREPEND "${CMAKE_CURRENT_BINARY_DIR}/" OUTPUT_VARIABLE res_out)
    add_custom_target(${resource_target}_resources DEPENDS ${res_out})
    add_dependencies(${resource_target} ${resource_target}_resources)
endfunction()

macro(add_phenyl_resource resource_path target_path)
    list(APPEND PHENYL_RESOURCE_LIST "${CMAKE_CURRENT_SOURCE_DIR}/${resource_path}")
    list(APPEND PHENYL_RESOURCE_TARGET_LIST ${target_path})

    set(PHENYL_RESOURCE_LIST ${PHENYL_RESOURCE_LIST} PARENT_SCOPE)
    set(PHENYL_RESOURCE_TARGET_LIST ${PHENYL_RESOURCE_TARGET_LIST} PARENT_SCOPE)
endmacro()

function(target_copy_phenyl_resources resource_target)
    list(LENGTH PHENYL_RESOURCE_LIST LEN)
    math(EXPR LEN "${LEN} - 1")
    set(res_out "")
    foreach (INDEX RANGE ${LEN})
        list(GET PHENYL_RESOURCE_LIST ${INDEX} path)
        list(GET PHENYL_RESOURCE_TARGET_LIST ${INDEX} res_target)

        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/${res_target}
            COMMAND
                cmake -E copy "${path}" "${CMAKE_CURRENT_BINARY_DIR}/${res_target}"
            DEPENDS
                ${path}
            VERBATIM
        )
        list(APPEND res_out ${CMAKE_CURRENT_BINARY_DIR}/${res_target})
        set(res_out ${res_out} PARENT_SCOPE)
    endforeach ()

    add_custom_target(${resource_target}_phenyl_resources DEPENDS ${res_out})
    add_dependencies(${resource_target} ${resource_target}_phenyl_resources)
endfunction()

function(phenyl_executable shared_target)
    add_executable(${shared_target}_exec ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/main/main.cpp)
    target_compile_definitions(${shared_target}_exec PRIVATE PHENYL_APP_LIB="./lib${shared_target}.so")
    target_link_libraries(${shared_target}_exec phenyl)
    set_property(TARGET ${shared_target}_exec PROPERTY CXX_STANDARD 20)
    add_dependencies(${shared_target}_exec ${shared_target})
endfunction()
