# Rocketbox2D
cmake_minimum_required(VERSION 4.0)
project(Rocketbox2D_Workspace VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

# Engine library
add_subdirectory(Engine)

option(RB2D_BUILD_ENGINE_TEST_APP "Build the engine test app" ON)

if(RB2D_BUILD_ENGINE_TEST_APP)
    add_subdirectory(EngineTestApp)
endif()