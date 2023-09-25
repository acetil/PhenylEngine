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
        )
        target_sources(${embed_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/${EMBED_FILE}.h)
    endforeach()
    target_include_directories(${embed_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated/)
endfunction()