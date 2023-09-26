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
                ${PYTHON_EXECUTABLE} ${EMBED_EXEC} ${EMBED_FILE} ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_FILE}.h
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