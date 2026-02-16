#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice* graphicsDevice, const BufferDesc& desc) {
        m_GraphicsDevice = graphicsDevice;
        m_BufferSize = desc.size;
        m_IsHostVisible = desc.isDynamic;
        m_Type = desc.type;

        // Create buffer
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.size = m_BufferSize;
        bufferInfo.usage = GetVulkanBufferUsage(desc.type);
        if(!m_IsHostVisible) {
            bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
        }
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;
        m_Buffer = vk::raii::Buffer(m_GraphicsDevice->m_Device->GetDevice(), bufferInfo);

        // Get memory requirements and properties
        vk::MemoryRequirements memRequirements = m_Buffer.getMemoryRequirements();
        vk::PhysicalDeviceMemoryProperties memProperties = m_GraphicsDevice->m_Context->GetPhysicalDevice().getMemoryProperties();

        // Allocate memory for buffer
        vk::MemoryAllocateInfo memoryAllocateInfo;
        memoryAllocateInfo.allocationSize = memRequirements.size;
        // Get flags

        vk::Flags<vk::MemoryPropertyFlagBits> memoryPropertyBits = 
            (m_IsHostVisible) 
            ? vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            : vk::MemoryPropertyFlagBits::eDeviceLocal;

        memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memProperties, memoryPropertyBits);
        m_BufferMemory = vk::raii::DeviceMemory( m_GraphicsDevice->m_Device->GetDevice(), memoryAllocateInfo );

        // Bind memory to buffer
        m_Buffer.bindMemory( *m_BufferMemory, 0 );

        if(m_IsHostVisible) {
            m_MappedPtr = m_BufferMemory.mapMemory(0, m_BufferSize);
        }

        // Write memory
        if(desc.data) {
            UpdateData(desc.data, desc.size, 0);
        }
    }

    void VulkanBuffer::UpdateData(const void* data, size_t size, size_t offset) {
        if(offset + size > m_BufferSize) {
            LOG_CORE_ERROR("Vulkan: Buffer overflow! Tried to write outside of buffer bounds.");
        }
        // IsHostVisible (Dynamic), mapMemory
        if (m_IsHostVisible) {
            // Write data to buffer directly
            memcpy((uint8_t*)m_MappedPtr + offset, data, size);
        } 
        // Not, staging buffer
        else {
            // I made a helper in m_GraphicsDevice so we don't have to hold a bunch of useless references
            m_GraphicsDevice->StageBufferUploadData(this, data, size, offset);
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        if(m_IsHostVisible) {
            m_BufferMemory.unmapMemory();
        }
    }

    vk::Flags<vk::BufferUsageFlagBits> VulkanBuffer::GetVulkanBufferUsage(BufferType type) {
        vk::BufferUsageFlags usage;
        
        switch(type) {
            case BufferType::Vertex:    usage = vk::BufferUsageFlagBits::eVertexBuffer; break;
            case BufferType::Index:     usage = vk::BufferUsageFlagBits::eIndexBuffer; break;
            case BufferType::Uniform:   usage = vk::BufferUsageFlagBits::eUniformBuffer; break;
        }

        return usage | vk::BufferUsageFlagBits::eTransferSrc;
    }

    uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, vk::PhysicalDeviceMemoryProperties memProperties, vk::MemoryPropertyFlags properties) {
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        LOG_CORE_ERROR("Vulkan: Failed to find suitable memory type when creating buffer!");
        return 0; // If stuff starts crashing, this is why
    }
} // namespace Engine
