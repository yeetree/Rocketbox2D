# Rocketbox2D
cmake_minimum_required(VERSION 4.0)
project(Rocketbox2D_Workspace VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Vulkan
find_package(Vulkan 1.4.335 REQUIRED)
find_package(VMA REQUIRED)
find_program(SLANGC_EXECUTABLE slangc REQUIRED)

# Vendored
add_subdirectory(vendor/glm)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/entt)
set(SPDLOG_BUILD_PIC ON)
add_subdirectory(vendor/spdlog)

# Helpers
include(EngineHelpers)
include(ScaryVoodooMagic)

# Engine
add_subdirectory(Engine)

# TestApp
add_subdirectory(VKTestApp)