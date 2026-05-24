#ifndef RHI_VULKAN_VULKANCONSTANTS
#define RHI_VULKAN_VULKANCONSTANTS

#include <vulkan/vulkan.hpp>
#include <array>

#ifdef ENGINE_DEBUG
constexpr const bool k_EnableValidationLayers = true;
#else
constexpr const bool k_EnableValidationLayers = false;
#endif

constexpr const std::array<char const*, 1> k_ValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

constexpr const std::array<const char*, 0 + k_EnableValidationLayers> k_InstanceExtensions = {
#ifdef ENGINE_DEBUG
    vk::EXTDebugUtilsExtensionName
#endif
};

constexpr const std::array<const char*, 4> k_DeviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRDynamicRenderingExtensionName
};

constexpr const int k_MaxFramesInFlight = 3;
constexpr const int k_MaxUniformBuffersPerFrame = 1000;
constexpr const int k_MaxSamplersPerFrame = 1000;
constexpr const int k_MaxDescriptorSetsPerFrame = 1000;

#endif // RHI_VULKAN_VULKANCONSTANTS
