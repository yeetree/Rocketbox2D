#include "RHI/Vulkan/VulkanCommon.h"

namespace Engine
{
    namespace VulkanCommon
    {
        void TransitionImageLayout(
            vk::raii::CommandBuffer& cmd,
            vk::Image               image,
            vk::ImageLayout         oldLayout,
            vk::ImageLayout         newLayout,
            vk::AccessFlags2        srcAccess,
            vk::AccessFlags2        dstAccess,
            vk::PipelineStageFlags2 srcStage,
            vk::PipelineStageFlags2 dstStage)
        {
            vk::ImageMemoryBarrier2 barrier;
            barrier.srcStageMask        = srcStage;
            barrier.srcAccessMask       = srcAccess;
            barrier.dstStageMask        = dstStage;
            barrier.dstAccessMask       = dstAccess;
            barrier.oldLayout           = oldLayout;
            barrier.newLayout           = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image               = image;
            barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = 1;

            vk::DependencyInfo dependencyInfo;
            dependencyInfo.imageMemoryBarrierCount = 1;
            dependencyInfo.pImageMemoryBarriers    = &barrier;
            
            cmd.pipelineBarrier2(dependencyInfo);
        }
    } // namespace VulkanCommon    
} // namespace Engine
