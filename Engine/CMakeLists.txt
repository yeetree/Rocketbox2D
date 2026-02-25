# Engine

include(GenerateExportHeader)

file(GLOB_RECURSE ENGINE_SOURCES 
    "source/*.cpp"
    "source/*.h"
)

add_library(Engine SHARED ${ENGINE_SOURCES})
add_library(Engine::Engine ALIAS Engine)

target_compile_definitions(Engine 
    PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 GLM_FORCE_DEPTH_ZERO_TO_ONE=1 ENGINE_DEBUG=1
)

target_include_directories(Engine
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/gen>
        $<INSTALL_INTERFACE:include>
    PRIVATE 
        source
        ../vendor/stb
)

target_link_libraries(Engine 
    PUBLIC 
        glm::glm 
        EnTT::EnTT 
        spdlog::spdlog
    PRIVATE 
        SDL3::SDL3-shared 
        Vulkan::Vulkan
        VMA::VMA
)

generate_export_header(Engine
    BASE_NAME ENGINE
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/gen/engine_export.h"
)

compile_shader_to_header(Engine "./assets/shaders/quad.slang" "${CMAKE_CURRENT_BINARY_DIR}/gen/generated/quadShader.h" "k_QuadShaderByteCode" "vertMain" "fragMain")

# RPath origin for Linux
set_target_properties(Engine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    INSTALL_RPATH "$ORIGIN"
)