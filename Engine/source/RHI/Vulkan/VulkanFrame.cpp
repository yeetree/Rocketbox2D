#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/VulkanConstants.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanFrame::VulkanFrame(VulkanContext* context)
    {
        // Create fence
        vk::FenceCreateInfo fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        m_Fence = vk::raii::Fence(context->GetDevice(), fenceInfo);

        // Command buffer pool
        m_CommandBufferPool = CreateScope<VulkanCommandBufferPool>(context);

        // Create dynamic buffers

        auto const& props = context->GetPhysicalDevice().getProperties();

        m_VertexDynamicBuffer = CreateScope<VulkanDynamicBuffer>(
            context,
            k_DynamicMegaBufferSize,
            vk::BufferUsageFlagBits::eVertexBuffer,
            16
        );
        m_IndexDynamicBuffer = CreateScope<VulkanDynamicBuffer>(
            context,
            k_DynamicMegaBufferSize,
            vk::BufferUsageFlagBits::eIndexBuffer,
            16
        );
        m_UniformDynamicBuffer = CreateScope<VulkanDynamicBuffer>(
            context,
            k_DynamicMegaBufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            props.limits.minUniformBufferOffsetAlignment
        );

        // Descriptor set allocator
        m_DescriptorSetAllocator = CreateScope<VulkanDescriptorSetAllocator>(context);
    }

    void VulkanFrame::Reset()
    {
        m_CommandBufferPool->Reset();
        m_VertexDynamicBuffer->Reset();
        m_IndexDynamicBuffer->Reset();
        m_UniformDynamicBuffer->Reset();
        m_DescriptorSetAllocator->Reset();
    }

    VulkanDynamicBuffer* VulkanFrame::GetDynamicBuffer(BufferType type)
    {
        switch(type)
        {
            case BufferType::Vertex: return m_VertexDynamicBuffer.get(); break;
            case BufferType::Index: return m_IndexDynamicBuffer.get(); break;
            case BufferType::Uniform: return m_UniformDynamicBuffer.get(); break;
        }
        return nullptr;
    }

    

} // namespace Engine
