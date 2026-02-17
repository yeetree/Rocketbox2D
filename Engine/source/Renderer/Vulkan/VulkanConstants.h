#ifndef RENDERER_VULKAN_VULKANCONSTANTS
#define RENDERER_VULKAN_VULKANCONSTANTS

#include <vulkan/vulkan.hpp>
#include <array>

#ifdef NDEBUG
const bool k_EnableValidationLayers = false;
#else
const bool k_EnableValidationLayers = true;
#endif

const std::array<char const*, 1> k_ValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::array<const char*, 4> k_DeviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

const int k_MaxFramesInFlight = 3;

#endif // RENDERER_VULKAN_VULKANCONSTANTS
