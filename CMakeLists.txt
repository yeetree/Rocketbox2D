# Rocketbox2D
cmake_minimum_required(VERSION 4.0)
project(Rocketbox2D_Workspace VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Vulkan
find_package(Vulkan 1.4.335 REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Vulkan Found: ${Vulkan_LIBRARY}")
    message(STATUS "Vulkan Version: ${Vulkan_VERSION}")
else()
    message(FATAL_ERROR "Vulkan SDK not found. Did you source setup-env.sh?")
endif()

find_path(VMA_INCLUDE_DIR 
    NAMES vk_mem_alloc.h 
    HINTS "${Vulkan_INCLUDE_DIRS}"
    PATH_SUFFIXES vma
)

if(NOT VMA_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find VMA (vk_mem_alloc.h) in ${Vulkan_INCLUDE_DIRS}")
endif()

# SlangC
find_program(SLANGC_EXECUTABLE slangc REQUIRED)

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

add_subdirectory(VKTestApp)