#include "RHI/Vulkan/VulkanDynamicBufferAllocator.h"
#include "Engine/Core/Assert.h"

namespace Engine::RHI::Vulkan
{
    VulkanDynamicBufferAllocator::VulkanDynamicBufferAllocator(VulkanContext& context, size_t totalSize, vk::BufferUsageFlags usage, size_t alignment)
        : m_Context(context), m_TotalSize(totalSize), m_Alignment(alignment)
    {
        if(m_TotalSize > 0)
        {
            // Buffer info
            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = m_TotalSize;
            bufferInfo.usage = usage;

            // Alloc info: map to CPU memory
            VmaAllocationCreateInfo allocInfo{};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

            VmaAllocationInfo resultInfo;
            VkBuffer buffer;
            vmaCreateBuffer(m_Context.GetAllocator(), bufferInfo, &allocInfo, &buffer, &m_Allocation, &resultInfo);

            m_Buffer = buffer;
            m_MappedData = resultInfo.pMappedData;
        }
    }

    VulkanDynamicBufferAllocator::~VulkanDynamicBufferAllocator()
    {
        if (m_Buffer)
        {
            vmaDestroyBuffer(m_Context.GetAllocator(), m_Buffer, m_Allocation);
            
            m_Buffer = nullptr;
            m_Allocation = nullptr;
            m_MappedData = nullptr;
        }
    }

    // Allocates space in mega buffer and returns start offset
    size_t VulkanDynamicBufferAllocator::AllocateAndCopy(void* data, size_t size)
    {
        // Align the current offset
        size_t alignedOffset = (m_CurrentOffset + m_Alignment - 1) & ~(m_Alignment - 1);

        // Make sure we have enough space
        ENGINE_CORE_ASSERT(alignedOffset + size <= m_TotalSize, "Vulkan: AllocateAndCopy: AllocateDynamicBuffer(): Dynamic Mega Buffer overflow!");

        char* offsetPtr = static_cast<char*>(m_MappedData) + alignedOffset;
        std::memcpy(offsetPtr, data, size);

        // Update offset
        m_CurrentOffset = alignedOffset + size;

        return alignedOffset;
    }

    void VulkanDynamicBufferAllocator::Reset()
    {
        m_CurrentOffset = 0;
    }
} // namespace Engine
