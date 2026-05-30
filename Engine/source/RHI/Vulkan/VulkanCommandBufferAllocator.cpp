#include "RHI/Vulkan/VulkanCommandBufferAllocator.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"

namespace Engine::RHI::Vulkan
{
    VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanContext& context)
        : m_Context(context)
    {
        // Create command pool
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            m_Context.GetGraphicsQueue().familyIndex
        );
        m_CommandPool = vk::raii::CommandPool(m_Context.GetDevice(), poolInfo);
    }

    VulkanCommandBufferAllocator::~VulkanCommandBufferAllocator() = default;

    VulkanCommandBuffer* VulkanCommandBufferAllocator::GetOrAllocate(VulkanGraphicsDevice& graphicsDevice)
    {
        // If we need a new command buffer, create a new one and add it to our list
        if (m_UsedCommandBuffers >= m_CommandBuffers.size())
        {
            m_CommandBuffers.push_back(
                CreateScope<VulkanCommandBuffer>(graphicsDevice, *m_CommandPool)
            );
        }

        VulkanCommandBuffer* cmd = m_CommandBuffers[m_UsedCommandBuffers++].get();
        cmd->Reset();
        return cmd;
    }

    void VulkanCommandBufferAllocator::Reset()
    {
        m_UsedCommandBuffers = 0;
    }
} // namespace Engine
