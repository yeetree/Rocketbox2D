#include "RHI/Vulkan/VulkanCommandBufferPool.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"

namespace Engine
{
    VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanContext* context)
    {
        // Create command pool
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            context->GetGraphicsQueue().familyIndex
        );
        m_CommandPool = vk::raii::CommandPool(context->GetDevice(), poolInfo);
    }

    VulkanCommandBuffer* VulkanCommandBufferPool::GetCommandBuffer(VulkanContext* context)
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

    void VulkanCommandBufferPool::Reset()
    {
        m_UsedCommandBuffers = 0;
    }
} // namespace Engine
