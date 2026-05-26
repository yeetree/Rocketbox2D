#include "RHI/Vulkan/VulkanFrame.h"

namespace Engine
{
    VulkanFrame::VulkanFrame(VulkanContext* context)
    {
        // Create fence
        vk::FenceCreateInfo fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        m_Fence = vk::raii::Fence(context->GetDevice(), fenceInfo);

        // Create command pool
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            context->GetGraphicsQueue().familyIndex
        );
        m_CommandPool = vk::raii::CommandPool(context->GetDevice(), poolInfo);
    }

    void VulkanFrame::Reset()
    {
        m_UsedCommandBuffers = 0;
    }

    VulkanCommandBuffer* VulkanFrame::GetCommandBuffer(VulkanContext* context)
    {
        // If we need a new command buffer, create a new one and add it to our list
        if (m_UsedCommandBuffers >= m_CommandBuffers.size())
        {
            Scope<VulkanCommandBuffer> vcmd = CreateScope<VulkanCommandBuffer>(context, *m_CommandPool);
            m_CommandBuffers.push_back(std::move(vcmd));
        }

        // Return command buffer
        VulkanCommandBuffer* cmd = m_CommandBuffers[m_UsedCommandBuffers].get();
        m_UsedCommandBuffers++;
        return cmd;
    }

} // namespace Engine
