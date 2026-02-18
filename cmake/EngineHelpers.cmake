# Engine Helpers

# Add project with engine. Handles linking and deploying
function(engine_add_project PROJECT_NAME SOURCES ASSET_DIR_IN ASSET_DIR_OUT)
    # Target
    add_executable(${PROJECT_NAME} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Engine)

    # Deploy executable
    set(APP_BIN_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")
    
    # Force Visual Studio to put the .exe in root of APP_BIN_DIR instead of APP_BIN_DIR/Debug or APP_BIN_DIR/Release
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${APP_BIN_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${APP_BIN_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${APP_BIN_DIR}"
        # Force linux to look for libraries in same directory
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    # Deployment

    # Copy dlls
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_BIN_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Engine::Engine>"
            "$<TARGET_FILE:SDL3::SDL3-shared>"
            "${APP_BIN_DIR}"
        COMMENT "Deployed DLL dependencies to ${APP_BIN_DIR}"
    )

    # Assets
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_DIR_IN}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/${ASSET_DIR_IN}"
                "${APP_BIN_DIR}/${ASSET_DIR_OUT}"
            COMMENT "Deployed assets to ${APP_BIN_DIR}/${ASSET_DIR_OUT}"
        )
    endif()
endfunction()

# Adds shaders
function(add_slang_shader PROJECT_NAME FILE_PATH OUTPUT_PATH VERT_ENTRY FRAG_ENTRY)
    # Set defaults
    if("${VERT_ENTRY}" STREQUAL "")
        set(VERT_ENTRY "vertMain")
    endif()
    if("${FRAG_ENTRY}" STREQUAL "")
        set(FRAG_ENTRY "fragMain")
    endif()

    set(OUTPUT_SPV "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}/${OUTPUT_PATH}")
    
    # Make sure output directory exists
    get_filename_component(OUT_DIR "${OUTPUT_SPV}" DIRECTORY)
    file(MAKE_DIRECTORY "${OUT_DIR}")

    # Get names of in/out files for comment
    get_filename_component(FILE_IN_NAME ${FILE_PATH} NAME)
    get_filename_component(SPV_OUT_NAME ${OUTPUT_SPV} NAME)
    
    # Unique target name
    string(MAKE_C_IDENTIFIER "shader_${PROJECT_NAME}_${SPV_OUT_NAME}" TARGET_NAME)

    # Shader comp via slangc
    add_custom_command(
        OUTPUT "${OUTPUT_SPV}"
        COMMAND ${SLANGC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}"
                -target spirv -profile spirv_1_5
                -emit-spirv-directly
                -entry ${VERT_ENTRY} 
                -entry ${FRAG_ENTRY}
                -o "${OUTPUT_SPV}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}"
        COMMENT "Building Shader: ${FILE_IN_NAME} -> ${SPV_OUT_NAME}"
        VERBATIM
    )
    
    # Build shaders target
    if(NOT TARGET all_shaders)
        add_custom_target(all_shaders ALL)
    endif()
    
    # Add to build shaders target
    add_custom_target(${TARGET_NAME} DEPENDS "${OUTPUT_SPV}")
    add_dependencies(all_shaders ${TARGET_NAME})
    
    # Make project depend on shader
    if(TARGET ${PROJECT_NAME})
        add_dependencies(${PROJECT_NAME} ${TARGET_NAME})
    endif()
endfunction()