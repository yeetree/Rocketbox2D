# Rocketbox2D
cmake_minimum_required(VERSION 4.0)
project(Rocketbox2D_Workspace VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenGL
find_package(OpenGL REQUIRED)

# GLM
add_subdirectory(vendor/glm)

# SDL3
set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
set(SDL_DISABLE_INSTALL ON)
add_subdirectory(vendor/SDL)

# EnTT
add_subdirectory(vendor/entt)

# spdlog
set(SPDLOG_BUILD_PIC ON)
add_subdirectory(vendor/spdlog)

# Engine library
add_subdirectory(Engine)

option(RB2D_BUILD_ENGINE_TEST_APP "Build the engine test app" ON)

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

if(RB2D_BUILD_ENGINE_TEST_APP)
    add_subdirectory(EngineTestApp)
endif()