#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

namespace Engine {
    VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice* graphicsDevice, const BufferDesc& desc) 
        : m_GraphicsDevice(graphicsDevice), 
          m_IsDynamic(desc.isDynamic), m_Size(desc.size)
    {
        ENGINE_CORE_ASSERT(graphicsDevice != nullptr, "Vulkan: invalid graphics device when creating buffer!");

        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = desc.size;
        bufferInfo.usage = GetVulkanBufferUsage(desc.type);
        VmaAllocationCreateInfo allocInfo = {};
        int buffersToMake = m_IsDynamic ? k_MaxFramesInFlight : 1;

        // Dynamic: Create one buffer per frame and keep memory mapped
        if(m_IsDynamic) {
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                              VMA_ALLOCATION_CREATE_MAPPED_BIT; 
        }
        // Static: Create one buffer and use staging buffer to upload
        else {
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        }

        m_Buffers.resize(buffersToMake);
    
        for(int i = 0; i < buffersToMake; i++) {
            VmaAllocationInfo resultInfo;
            vmaCreateBuffer(
                m_GraphicsDevice->GetDevice().GetAllocator(), 
                &bufferInfo, 
                &allocInfo, 
                &m_Buffers[i].buffer, 
                &m_Buffers[i].allocation,
                &resultInfo
            );
            if(m_IsDynamic) {
                m_Buffers[i].mapPointer = resultInfo.pMappedData;
            }
        }

    
        if (desc.data != nullptr) {
            if (m_IsDynamic) {
                // init all frames
                for (int i = 0; i < buffersToMake; i++) {
                    memcpy((uint8_t*)m_Buffers[i].mapPointer, desc.data, desc.size);
                }
            } else {
                UpdateData(desc.data, desc.size, 0);
            }
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        // VMA doesn't use raii, destroy manually
        for(const BufferInfo& buffer : m_Buffers) {
            if(buffer.buffer) {
                vmaDestroyBuffer(m_GraphicsDevice->GetDevice().GetAllocator(), buffer.buffer, buffer.allocation);
            }
        }
    }

    void VulkanBuffer::UpdateData(const void* data, size_t size, size_t offset) {
        if(data == nullptr || size < 1) {
            return;
        }
        
        // Dynamic: Memory is mapped, get frame index and copy buffer data
        if (m_IsDynamic) {
            memcpy((uint8_t*)m_Buffers[m_GraphicsDevice->GetFrameIndex()].mapPointer + offset, data, size);
        // Static: Create staging buffer
        } else {

            BufferInfo stagingBuffer;

            VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
            bufferInfo.size = size;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                              VMA_ALLOCATION_CREATE_MAPPED_BIT; 


            VmaAllocationInfo resultInfo;
            vmaCreateBuffer(
                m_GraphicsDevice->GetDevice().GetAllocator(), 
                &bufferInfo, 
                &allocInfo, 
                &stagingBuffer.buffer, 
                &stagingBuffer.allocation,
                &resultInfo
            );
            stagingBuffer.mapPointer = resultInfo.pMappedData;

            memcpy(resultInfo.pMappedData, data, size);
        
            // Copy
            vk::raii::CommandBuffer cmd = m_GraphicsDevice->BeginOneTimeCommands();

            vk::BufferCopy copyRegion(0, offset, size);
            cmd.copyBuffer(
                static_cast<vk::Buffer>(stagingBuffer.buffer), 
                static_cast<vk::Buffer>(m_Buffers[0].buffer), 
                copyRegion
            );

            m_GraphicsDevice->EndOneTimeCommands(cmd);

            // Destroy
            vmaDestroyBuffer(m_GraphicsDevice->GetDevice().GetAllocator(), stagingBuffer.buffer, stagingBuffer.allocation);
        }
    }

    size_t VulkanBuffer::GetSize() const {
        return m_Size;
    }

    VkBuffer& VulkanBuffer::GetBuffer() {
        return m_Buffers[(m_IsDynamic) ? m_GraphicsDevice->GetFrameIndex() : 0].buffer;
    }

    VkBufferUsageFlags VulkanBuffer::GetVulkanBufferUsage(BufferType type) {
        switch(type) {
            case BufferType::Vertex:  return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; break;
            case BufferType::Index:   return VK_BUFFER_USAGE_INDEX_BUFFER_BIT; break;
            case BufferType::Uniform: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; break;
        }
        return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
}