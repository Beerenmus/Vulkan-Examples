function(compile_shaders TARGET_NAME)

    set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/shaders/${TARGET_NAME})

    file(GLOB SHADERS
            ${SHADER_SRC_DIR}/*.vert
            ${SHADER_SRC_DIR}/*.frag
    )

    set(SPIRV_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

    set(SPIRV_BINARIES)

    foreach(SHADER ${SHADERS})
        message(STATUS "Found: ${SHADER}")
        get_filename_component(shader_name ${SHADER} NAME)
        set(OUTPUT_FILE ${SPIRV_OUTPUT_DIR}/${shader_name}.spv)

        add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND glslc ${SHADER} -o ${OUTPUT_FILE}
                DEPENDS ${SHADER}
                COMMENT "Compiling ${shader_name} -> SPIR-V"
        )

        list(APPEND SPIRV_BINARIES ${OUTPUT_FILE})
    endforeach()

    add_custom_target(${TARGET_NAME}_SHADERS ALL DEPENDS ${SPIRV_BINARIES})
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_SHADERS)

endfunction()