#ifndef RHI_VULKAN_VULKANCOMMON
#define RHI_VULKAN_VULKANCOMMON

#include "engine_export.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    namespace VulkanCommon
    {
        ENGINE_EXPORT void TransitionImageLayout(
            vk::raii::CommandBuffer& cmd,
            vk::Image               image,
            vk::ImageLayout         oldLayout,
            vk::ImageLayout         newLayout,
            vk::AccessFlags2        srcAccess,
            vk::AccessFlags2        dstAccess,
            vk::PipelineStageFlags2 srcStage,
            vk::PipelineStageFlags2 dstStage);
    } // namespace VulkanCommon
} // namespace Engine


#endif // RHI_VULKAN_VULKANCOMMON
