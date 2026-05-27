#include "RHI/Vulkan/RHI/VulkanBuffer.h"
#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "RHI/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    vk::BufferUsageFlags VulkanBuffer::GetBufferUsageFlags(BufferType type)
    {
        vk::BufferUsageFlags flags;
        switch(type)
        {
            case BufferType::Vertex: flags = vk::BufferUsageFlagBits::eVertexBuffer; break;
            case BufferType::Index: flags = vk::BufferUsageFlagBits::eIndexBuffer; break;
        }
        return flags;
    }

    VulkanBuffer::VulkanBuffer(VulkanContext* context, const BufferDesc& desc)
        : IBuffer(desc.size, desc.type, desc.usage)
    {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: VulkanBuffer(): context is nullptr!");

        switch(desc.usage)
        {
            // Static buffers:
            // Allocate buffer using VMA
            case BufferUsage::Static:
            {
                // Buffer info
                vk::BufferCreateInfo bufferInfo;
                bufferInfo.size = desc.size;
                bufferInfo.usage = GetBufferUsageFlags(desc.type) | vk::BufferUsageFlagBits::eTransferDst;

                // Alloc info: map to CPU memory
                VmaAllocationCreateInfo allocInfo{};
                allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

                m_Allocator = context->GetAllocator();

                VmaAllocationInfo resultInfo;
                VkBuffer buffer;
                vmaCreateBuffer(m_Allocator, bufferInfo, &allocInfo, &buffer, &m_Allocation, &resultInfo);

                m_Buffer = buffer;
                break;
            }

            // Dynamic buffers:
            // Resize m_DynamicOffsets.
            // Buffers are chunks of a VulkanDynamicBuffer in the VulkanFrame.
            case BufferUsage::Dynamic:
            {
                m_DynamicOffsets.resize(k_MaxFramesInFlight);
                break;
            }
        }
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if(GetUsage() == BufferUsage::Static && m_Buffer)
        {
            vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
        }
    }

    size_t VulkanBuffer::GetOffset(uint32_t frameIdx)
    {
        return (GetUsage() == BufferUsage::Static) ? 0 : m_DynamicOffsets[frameIdx];
    }

    void VulkanBuffer::SetData(VulkanFrame* frame, uint32_t frameIdx, void* data, size_t size)
    {
        if(GetUsage() != BufferUsage::Dynamic)
        {
            LOG_CORE_ERROR("VulkanBuffer: SetData(): Buffer is static!");
            return;
        }

        m_DynamicOffsets[frameIdx] = frame->GetDynamicBuffer(GetType())->AllocateAndCopy(data, size);
    }
} // namespace Engine
