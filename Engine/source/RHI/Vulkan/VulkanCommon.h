#ifndef RHI_VULKAN_VULKANCOMMON
#define RHI_VULKAN_VULKANCOMMON

#include "engine_export.h"

#include "Engine/RHI/RHI.h"

#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace Engine::RHI::Vulkan::VulkanCommon
{
    ENGINE_EXPORT void TransitionImageLayout(
        vk::CommandBuffer        cmd,
        vk::Image                image,
        vk::ImageLayout          oldLayout,
        vk::ImageLayout          newLayout,
        vk::AccessFlags2         srcAccess,
        vk::AccessFlags2         dstAccess,
        vk::PipelineStageFlags2  srcStage,
        vk::PipelineStageFlags2  dstStage
    );

    ENGINE_EXPORT vk::SurfaceFormatKHR GetSurfaceFormat(PixelFormat format);

    // Returns vk::SurfaceFormatKHR.format = vk::Format::eUndefined if format is not supported
    ENGINE_EXPORT vk::SurfaceFormatKHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, vk::SurfaceFormatKHR requestedFormat);

    ENGINE_EXPORT vk::PresentModeKHR GetPresentMode(PresentMode presentMode);

    // Falls back to vk::PresentModeKHR::eImmediate if present mode is not supported
    ENGINE_EXPORT vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& availableModes, vk::PresentModeKHR requestedMode);

    ENGINE_EXPORT vk::Extent2D GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    ENGINE_EXPORT uint32_t GetSurfaceMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);
} // namespace Engine::RHI::Vulkan::VulkanCommon


#endif // RHI_VULKAN_VULKANCOMMON
