#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"

namespace Engine {
    VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice* graphicsDevice, const BufferDesc& desc) 
        : m_GraphicsDevice(graphicsDevice), m_BufferSize(desc.size), 
          m_IsHostVisible(desc.isDynamic), m_Type(desc.type) 
    {
        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = m_BufferSize;
        bufferInfo.usage = GetVulkanBufferUsage(desc.type);
        
        // If not dynamic, set able to be copied to
        if(!m_IsHostVisible) {
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        VmaAllocationCreateInfo allocInfo = {};
        if (m_IsHostVisible) {
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                              VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        } else {
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        }

        VmaAllocationInfo resultInfo;
        vmaCreateBuffer(
            m_GraphicsDevice->m_Device->GetAllocator(), 
            &bufferInfo, 
            &allocInfo, 
            &m_Buffer, 
            &m_Allocation, 
            &resultInfo
        );

        // Get VMA mapped pointer
        if (m_IsHostVisible) {
            m_MappedPtr = resultInfo.pMappedData;
        }

        if (desc.data) {
            UpdateData(desc.data, desc.size, 0);
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        // VMA doesn't use raii, destroy manually
        if (m_Buffer) {
            vmaDestroyBuffer(m_GraphicsDevice->m_Device->GetAllocator(), m_Buffer, m_Allocation);
        }
    }

    void VulkanBuffer::UpdateData(const void* data, size_t size, size_t offset) {
        if (m_IsHostVisible) {
            memcpy((uint8_t*)m_MappedPtr + offset, data, size);
        } else {
            m_GraphicsDevice->StageBufferUploadData(this, data, size, offset);
        }
    }

    VkBufferUsageFlags VulkanBuffer::GetVulkanBufferUsage(BufferType type) {
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        switch(type) {
            case BufferType::Vertex:  usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
            case BufferType::Index:   usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
            case BufferType::Uniform: usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
        }
        return usage;
    }
}