# Rocketbox2D Engine FindVMA.cmake
# Only works if VMA is installed in VulkanSDK

include(FindPackageHandleStandardArgs)

# Check the Vulkan SDK first, then any includes
find_path(VMA_INCLUDE_DIR
    NAMES vk_mem_alloc.h
    HINTS 
        ${Vulkan_INCLUDE_DIRS}      # From FindVulkan
        $ENV{VULKAN_SDK}/include    # VULKAN_SDK environment variable
    PATH_SUFFIXES 
        vma
        VulkanMemoryAllocator
)

find_package_handle_standard_args(VMA
    REQUIRED_VARS VMA_INCLUDE_DIR
)

# Create target
if(VMA_FOUND AND NOT TARGET VMA::VMA)
    add_library(VMA::VMA INTERFACE IMPORTED)
    target_include_directories(VMA::VMA INTERFACE "${VMA_INCLUDE_DIR}")
    
    # VMA requires Vulkan
    if(TARGET Vulkan::Vulkan)
        target_link_libraries(VMA::VMA INTERFACE Vulkan::Vulkan)
    endif()
endif()

mark_as_advanced(VMA_INCLUDE_DIR)