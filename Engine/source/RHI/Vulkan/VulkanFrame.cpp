#include "RHI/Vulkan/VulkanFrame.h"
#include "RHI/Vulkan/VulkanConstants.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "Engine/Core/Assert.h"

namespace Engine::RHI::Vulkan
{
    VulkanFrame::VulkanFrame(VulkanContext& context)
        : m_CommandBufferAllocator(context),
          m_VertexDynamicBufferAllocator(
            context, 
            k_VertexDynamicBufferSizePerFrame, 
            vk::BufferUsageFlagBits::eVertexBuffer,
            64
          ),
          m_IndexDynamicBufferAllocator(
            context, 
            k_VertexDynamicBufferSizePerFrame, 
            vk::BufferUsageFlagBits::eIndexBuffer,
            64
          ),
          m_UniformDynamicBufferAllocator(
            context, 
            k_VertexDynamicBufferSizePerFrame, 
            vk::BufferUsageFlagBits::eUniformBuffer,
            context.GetPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment
          ),
          m_DescriptorSetAllocator(context)
    {
        // Create fence
        vk::FenceCreateInfo fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        m_Fence = vk::raii::Fence(context.GetDevice(), fenceInfo);
    }

    VulkanFrame::~VulkanFrame()
    {
      Reset();
    }

    void VulkanFrame::Reset()
    {
        m_CommandBufferAllocator.Reset();
        m_VertexDynamicBufferAllocator.Reset();
        m_IndexDynamicBufferAllocator.Reset();
        m_UniformDynamicBufferAllocator.Reset();
        m_DescriptorSetAllocator.Reset();
    }    

} // namespace Engine::RHI::Vulkan
