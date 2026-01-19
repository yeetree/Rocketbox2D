# Engine

include(GenerateExportHeader)

# Engine sources
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/FileSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/Input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/Log.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/RHI/IGraphicsDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/RHI/IShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/RHI/VertexLayout.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/Vulkan/VulkanGraphicsDevice.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/Camera.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Scene/Scene.cpp
)

# Engine includes
set(ENGINE_PUBLIC_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/gen
     ${CMAKE_CURRENT_SOURCE_DIR}../vendor/glm
)

set(ENGINE_PRIVATE_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/stb
)

# Build and link Engine
add_library(Engine SHARED ${ENGINE_SOURCES})
add_library(Engine::Engine ALIAS Engine)
target_link_libraries(Engine PUBLIC glm::glm SDL3::SDL3 EnTT::EnTT spdlog::spdlog PRIVATE Vulkan::Vulkan)
target_compile_definitions(Engine PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_include_directories(Engine PUBLIC ${ENGINE_PUBLIC_INCLUDES} PRIVATE ${ENGINE_PRIVATE_INCLUDES})

# Hide all symbols by default
set_target_properties(Engine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Generate export header
# use "#include "engine_export.h"" and ENGINE_EXPORT macro
generate_export_header(Engine
    BASE_NAME ENGINE
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/gen/engine_export.h"
)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL3::SDL3>"
    "${CMAKE_BINARY_DIR}/bin"
    COMMENT "Copying SDL3 library to common bin..."
)

# Function to add project
function(engine_add_project PROJECT_NAME SOURCES ASSET_DIR)
    # Create the executable
    add_executable(${PROJECT_NAME} ${SOURCES})
    
    # Set the custom output directory
    set(APP_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${APP_OUTPUT_DIR}"
        # Find .so files in the same folder
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    # Link the engine
    target_link_libraries(${PROJECT_NAME} PRIVATE Engine::Engine)

    # Post-Build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_OUTPUT_DIR}"
        # Copy the Engine and SDL libraries
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Engine::Engine>"
            "$<TARGET_FILE:SDL3::SDL3>"
            "${APP_OUTPUT_DIR}"
        # Copy assets if the directory exists
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSET_DIR}"
            "${APP_OUTPUT_DIR}/Assets"
        COMMENT "Deployed ${PROJECT_NAME} to ${APP_OUTPUT_DIR}"
    )
endfunction()

# function to add shader target
function(add_slang_shader_target TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

    set(SHADERS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    set(OUTPUT_SPV "${SHADERS_OUTPUT_DIR}/slang.spv")
    
    file(MAKE_DIRECTORY "${SHADERS_OUTPUT_DIR}")

    set(ENTRY_POINTS -entry vertMain -entry fragMain)

    add_custom_command(
        OUTPUT "${OUTPUT_SPV}"
        COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} 
                -target spirv 
                -profile spirv_1_4 
                -emit-spirv-directly 
                -fvk-use-entrypoint-name 
                ${ENTRY_POINTS} 
                -o "${OUTPUT_SPV}"
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling Slang Shaders: ${SHADER_SOURCES}"
        VERBATIM
    )

    add_custom_target(${TARGET} DEPENDS "${OUTPUT_SPV}")
endfunction()