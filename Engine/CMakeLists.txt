# Engine

include(GenerateExportHeader)

# Engine sources
set(ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/glad/src/gl.c

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Core/Engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/IGraphicsDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/VertexLayout.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/OpenGL/OpenGLGraphicsDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/OpenGL/OpenGLPipelineState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/OpenGL/OpenGLBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/OpenGL/OpenGLShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Renderer/OpenGL/OpenGLTexture.cpp
)

# Engine includes
set(ENGINE_PUBLIC_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/gen
     ${CMAKE_CURRENT_SOURCE_DIR}../vendor/glm
)

set(ENGINE_PRIVATE_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/glad/include
)

# Build and link Engine
add_library(Engine SHARED ${ENGINE_SOURCES})
add_library(Engine::Engine ALIAS Engine)
target_link_libraries(Engine PUBLIC glm::glm SDL3::SDL3 PRIVATE OpenGL::GL)
target_include_directories(Engine PUBLIC ${ENGINE_PUBLIC_INCLUDES} PRIVATE ${ENGINE_PRIVATE_INCLUDES})

# Hide all symbols by default
set_target_properties(Engine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

# Generate export header
# use "#include "engine_export.h"" and ENGINE_EXPORT macro
generate_export_header(Engine
    BASE_NAME ENGINE
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/gen/engine_export.h"
)

# Copy SDL3 library
add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL3::SDL3>"
    "$<TARGET_FILE_DIR:Engine>"
    COMMENT "Copying SDL3 library..."
)