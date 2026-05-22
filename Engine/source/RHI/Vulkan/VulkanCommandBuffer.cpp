#include "RHI/Vulkan/VulkanCommandBuffer.h"

namespace Engine
{
    VulkanCommandBuffer::VulkanCommandBuffer(Ref<VulkanContext> context, const vk::raii::CommandPool& commandPool)
        : m_Context(context),
          m_CommandBuffer(std::move(vk::raii::CommandBuffers(context->GetDevice(), {*commandPool, vk::CommandBufferLevel::ePrimary, 1}).front()))
    {
    }

    void VulkanCommandBuffer::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_CommandBuffer.begin(beginInfo);
    }

    void VulkanCommandBuffer::End()
    {
        m_CommandBuffer.end();
    }
} // namespace Engine
