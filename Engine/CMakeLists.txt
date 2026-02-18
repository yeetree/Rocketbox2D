# Engine

include(GenerateExportHeader)

file(GLOB_RECURSE ENGINE_SOURCES 
    "source/*.cpp"
    "source/*.h"
)

add_library(Engine SHARED ${ENGINE_SOURCES})
add_library(Engine::Engine ALIAS Engine)

target_compile_definitions(Engine 
    PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
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
        SDL3::SDL3-shared 
        EnTT::EnTT 
        spdlog::spdlog
    PRIVATE 
        Vulkan::Vulkan
        VMA::VMA
)

generate_export_header(Engine
    BASE_NAME ENGINE
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/gen/engine_export.h"
)

# RPath origin for Linux
set_target_properties(Engine PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    INSTALL_RPATH "$ORIGIN"
)